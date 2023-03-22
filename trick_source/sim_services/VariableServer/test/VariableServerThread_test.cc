/******************************TRICK HEADER*************************************
PURPOSE:                     ( Tests for the VariableServerThread class )
*******************************************************************************/

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <iostream>
#include <iomanip>
#include <limits>
#include <vector>

#include "trick/MemoryManager.hh"
#include "trick/UdUnits.hh"
#include "trick/VariableServer.hh"
#include "trick/VariableServerThread.hh"
#include "trick/var_binary_parser.hh"

#include "MockVariableServerSession.hh"
#include "MockClientConnection.hh"

using ::testing::Return;
using ::testing::_;
using ::testing::AtLeast;
using ::testing::DoAll;
using ::testing::Throw;
using ::testing::Const;
using ::testing::NiceMock;


// Set up default mock behavior

void setup_default_connection_mocks (MockClientConnection * connection) {
    // Starting the connection succeeds
    ON_CALL(*connection, start())
        .WillByDefault(Return(0));
    
    // We should always get a disconnect call
    ON_CALL(*connection, disconnect())
        .WillByDefault(Return(0));
}

void setup_default_session_mocks (MockVariableServerSession * session, bool command_exit = true) {
    ON_CALL(*session, handle_message())
        .WillByDefault(Return(0));

    ON_CALL(*session, copy_sim_data())
        .WillByDefault(Return(0));

    ON_CALL(*session, get_pause())
        .WillByDefault(Return(false));

    ON_CALL(*session, write_data())
        .WillByDefault(Return(0));

    ON_CALL(Const(*session), get_copy_mode())
        .WillByDefault(Return(VS_COPY_ASYNC));

    ON_CALL(Const(*session), get_write_mode())
        .WillByDefault(Return(VS_WRITE_ASYNC));

    ON_CALL(Const(*session), get_update_rate())
        .WillByDefault(Return(0.01));

    ON_CALL(*session, get_exit_cmd())
        .WillByDefault(Return(false));
}

/*
 Test Fixture.
 */
class VariableServerThread_test : public ::testing::Test {
	protected:
	    Trick::MemoryManager * memmgr;
        Trick::VariableServer * varserver;

        MockClientConnection connection;
        NiceMock<MockVariableServerSession> * session;

		VariableServerThread_test() { 
            memmgr = new Trick::MemoryManager; 
            varserver = new Trick::VariableServer;
            session = new  NiceMock<MockVariableServerSession>;

            Trick::VariableServerThread::set_vs_ptr(varserver);

            setup_default_connection_mocks(&connection);
            setup_default_session_mocks(session);
        }

		~VariableServerThread_test() { 
            delete memmgr;
            delete varserver; 
        }

		void SetUp() {}
		void TearDown() {}
};


// Helper functions for common cases of Mock expectations

void setup_normal_connection_expectations (MockClientConnection * connection) {
    // Starting the connection succeeds
    EXPECT_CALL(*connection, start())
        .Times(1)
        .WillOnce(Return(0));
    
    // We should always get a disconnect call
    EXPECT_CALL(*connection, disconnect())
        .Times(1);
}

void set_session_exit_after_some_loops(MockVariableServerSession * session) {
    EXPECT_CALL(*session, get_exit_cmd())
        .WillOnce(Return(false))
        .WillOnce(Return(false))
        .WillOnce(Return(false))
        .WillOnce(Return(false))
        .WillOnce(Return(false))
        .WillOnce(Return(true));
}

TEST_F(VariableServerThread_test, connection_failure) {
    // ARRANGE

    // Starting the connection fails, disconnect should be called
    EXPECT_CALL(connection, start())
        .Times(1)
        .WillOnce(Return(1));
    EXPECT_CALL(connection, disconnect())
        .Times(1);
    
    // Set up VariableServerThread
    Trick::VariableServerThread * vst = new Trick::VariableServerThread(session) ;
    vst->set_connection(&connection);

    // ACT
    vst->create_thread();
    pthread_t id = vst->get_pthread_id();
    Trick::ConnectionStatus status = vst->wait_for_accept();
    vst->join_thread();

    // ASSERT
    EXPECT_EQ(status, Trick::ConnectionStatus::CONNECTION_FAIL);

    // There should be nothing in the VariableServer's thread list
    EXPECT_EQ(varserver->get_vst(id), (Trick::VariableServerThread *) NULL);
    EXPECT_EQ(varserver->get_session(id), (Trick::VariableServerSession *) NULL);
}


TEST_F(VariableServerThread_test, exit_if_handle_message_fails) {
    // ARRANGE
    setup_normal_connection_expectations(&connection);
    
    // Handle a message, but it fails
    EXPECT_CALL(*session, handle_message())
        .Times(1)
        .WillOnce(Return(-1));
    
        
    // Set up VariableServerThread
    Trick::VariableServerThread * vst = new Trick::VariableServerThread(session) ;
    vst->set_connection(&connection);

    // ACT
    vst->create_thread();
    pthread_t id = vst->get_pthread_id();
    Trick::ConnectionStatus status = vst->wait_for_accept();
    ASSERT_EQ(status, Trick::ConnectionStatus::CONNECTION_SUCCESS);

    vst->join_thread();

    // ASSERT

    // There should be nothing in the VariableServer's thread list
    EXPECT_EQ(varserver->get_vst(id), (Trick::VariableServerThread *) NULL);
    EXPECT_EQ(varserver->get_session(id), (Trick::VariableServerSession *) NULL);
}


TEST_F(VariableServerThread_test, exit_if_write_fails) {
    // ARRANGE
    setup_normal_connection_expectations(&connection);
    
    // Write out data
    EXPECT_CALL(*session, write_data())
        .Times(1)
        .WillOnce(Return(-1));
    
        
    // Set up VariableServerThread
    Trick::VariableServerThread * vst = new Trick::VariableServerThread(session) ;
    vst->set_connection(&connection);

    // ACT
    vst->create_thread();
    pthread_t id = vst->get_pthread_id();
    Trick::ConnectionStatus status = vst->wait_for_accept();
    ASSERT_EQ(status, Trick::ConnectionStatus::CONNECTION_SUCCESS);

    vst->join_thread();

    // ASSERT

    // There should be nothing in the VariableServer's thread list
    EXPECT_EQ(varserver->get_vst(id), (Trick::VariableServerThread *) NULL);
    EXPECT_EQ(varserver->get_session(id), (Trick::VariableServerSession *) NULL);
}

TEST_F(VariableServerThread_test, exit_commanded) {
    // ARRANGE
    setup_normal_connection_expectations(&connection);
    set_session_exit_after_some_loops(session);

    // Set up VariableServerThread
    Trick::VariableServerThread * vst = new Trick::VariableServerThread(session) ;
    vst->set_connection(&connection);

    // ACT
    vst->create_thread();
    pthread_t id = vst->get_pthread_id();
    Trick::ConnectionStatus status = vst->wait_for_accept();
    ASSERT_EQ(status, Trick::ConnectionStatus::CONNECTION_SUCCESS);

    // Confirm that the session has been created
    Trick::VariableServerSession * vs_session = varserver->get_session(id);
    ASSERT_TRUE(vs_session == session);

    // Runs for a few loops, then exits

    // Thread should shut down
    vst->join_thread();

    // ASSERT
    // There should be nothing in the VariableServer's thread list
    EXPECT_EQ(varserver->get_vst(id), (Trick::VariableServerThread *) NULL);
    EXPECT_EQ(varserver->get_session(id), (Trick::VariableServerSession *) NULL);
}

TEST_F(VariableServerThread_test, thread_cancelled) {
    // ARRANGE
    setup_normal_connection_expectations(&connection);
    
    // Set up VariableServerThread
    Trick::VariableServerThread * vst = new Trick::VariableServerThread(session) ;
    vst->set_connection(&connection);
    vst->create_thread();
    pthread_t id = vst->get_pthread_id();
    Trick::ConnectionStatus status = vst->wait_for_accept();
    ASSERT_EQ(status, Trick::ConnectionStatus::CONNECTION_SUCCESS);

    // Confirm that the session has been created
    Trick::VariableServerSession * vs_session = varserver->get_session(id);
    ASSERT_TRUE(vs_session == session);

    // ACT
    vst->cancel_thread();

    // Thread should shut down
    vst->join_thread();

    // ASSERT
    // There should be nothing in the VariableServer's thread list
    EXPECT_EQ(varserver->get_vst(id), (Trick::VariableServerThread *) NULL);
    EXPECT_EQ(varserver->get_session(id), (Trick::VariableServerSession *) NULL);
}

TEST_F(VariableServerThread_test, no_write_if_paused) {
    // ARRANGE
    setup_normal_connection_expectations(&connection);
    set_session_exit_after_some_loops(session);

    // We're paused the whole time
    EXPECT_CALL(*session, get_pause())
        .WillRepeatedly(Return(true));

    // We should never write
    EXPECT_CALL(*session, write_data())
        .Times(0);
    
    // Set up VariableServerThread
    Trick::VariableServerThread * vst = new Trick::VariableServerThread(session) ;
    vst->set_connection(&connection);

    // ACT
    vst->create_thread();
    pthread_t id = vst->get_pthread_id();
    Trick::ConnectionStatus status = vst->wait_for_accept();
    ASSERT_EQ(status, Trick::ConnectionStatus::CONNECTION_SUCCESS);

    // Confirm that the session has been created
    Trick::VariableServerSession * vs_session = varserver->get_session(id);
    ASSERT_TRUE(vs_session == session);

    // Thread should shut down
    vst->join_thread();

    // ASSERT
    // There should be nothing in the VariableServer's thread list
    EXPECT_EQ(varserver->get_vst(id), (Trick::VariableServerThread *) NULL);
    EXPECT_EQ(varserver->get_session(id), (Trick::VariableServerSession *) NULL);
}


TEST_F(VariableServerThread_test, no_copy_if_wrong_mode) {
    // ARRANGE
    setup_normal_connection_expectations(&connection);
    set_session_exit_after_some_loops(session);

    // We're in copy_scheduled mode
    EXPECT_CALL(Const(*session), get_copy_mode())
        .WillRepeatedly(Return(VS_COPY_SCHEDULED));

    // We should never copy
    EXPECT_CALL(*session, copy_sim_data())
        .Times(0);
    
    // Set up VariableServerThread
    Trick::VariableServerThread * vst = new Trick::VariableServerThread(session) ;
    vst->set_connection(&connection);

    // ACT
    vst->create_thread();
    pthread_t id = vst->get_pthread_id();
    Trick::ConnectionStatus status = vst->wait_for_accept();
    ASSERT_EQ(status, Trick::ConnectionStatus::CONNECTION_SUCCESS);

    // Confirm that the session has been created
    Trick::VariableServerSession * vs_session = varserver->get_session(id);
    ASSERT_TRUE(vs_session == session);

    // Thread should shut down
    vst->join_thread();

    // ASSERT
    // There should be nothing in the VariableServer's thread list
    EXPECT_EQ(varserver->get_vst(id), (Trick::VariableServerThread *) NULL);
    EXPECT_EQ(varserver->get_session(id), (Trick::VariableServerSession *) NULL);
}

TEST_F(VariableServerThread_test, write_when_copied) {
    // ARRANGE
    setup_normal_connection_expectations(&connection);
    set_session_exit_after_some_loops(session);

    // Default copy is copy_async, leave that be
    // We're in write_when_copied
    EXPECT_CALL(Const(*session), get_write_mode())
        .WillRepeatedly(Return(VS_WRITE_WHEN_COPIED));

    // We should still be copying and writing
    EXPECT_CALL(*session, copy_sim_data())
        .Times(AtLeast(1));
    
    EXPECT_CALL(*session, write_data())
        .Times(AtLeast(1));

    // Set up VariableServerThread
    Trick::VariableServerThread * vst = new Trick::VariableServerThread(session) ;
    vst->set_connection(&connection);

    // ACT
    vst->create_thread();
    pthread_t id = vst->get_pthread_id();
    Trick::ConnectionStatus status = vst->wait_for_accept();
    ASSERT_EQ(status, Trick::ConnectionStatus::CONNECTION_SUCCESS);

    // Confirm that the session has been created
    Trick::VariableServerSession * vs_session = varserver->get_session(id);
    ASSERT_TRUE(vs_session == session);

    // Thread should shut down
    vst->join_thread();

    // ASSERT
    // There should be nothing in the VariableServer's thread list
    EXPECT_EQ(varserver->get_vst(id), (Trick::VariableServerThread *) NULL);
    EXPECT_EQ(varserver->get_session(id), (Trick::VariableServerSession *) NULL);
}


// There's a problem here - the VariableServerSession has an unwrapped connection to the executive
// So we can't hook into the is_real_time function
TEST_F(VariableServerThread_test, DISABLED_write_when_copied_sync) {
    // ARRANGE
    setup_normal_connection_expectations(&connection);
    set_session_exit_after_some_loops(session);

    // Copy scheduled, so not here
    EXPECT_CALL(Const(*session), get_copy_mode())
        .WillRepeatedly(Return(VS_COPY_SCHEDULED));
    // We're in write_when_copied
    EXPECT_CALL(Const(*session), get_write_mode())
        .WillRepeatedly(Return(VS_WRITE_WHEN_COPIED));

    // No copying or writing
    EXPECT_CALL(*session, copy_sim_data())
        .Times(0);
    
    EXPECT_CALL(*session, write_data())
        .Times(0);

    // Set up VariableServerThread
    Trick::VariableServerThread * vst = new Trick::VariableServerThread(session) ;
    vst->set_connection(&connection);

    // ACT
    vst->create_thread();
    pthread_t id = vst->get_pthread_id();
    Trick::ConnectionStatus status = vst->wait_for_accept();
    ASSERT_EQ(status, Trick::ConnectionStatus::CONNECTION_SUCCESS);

    // Confirm that the session has been created
    Trick::VariableServerSession * vs_session = varserver->get_session(id);
    ASSERT_TRUE(vs_session == session);

    // Thread should shut down
    vst->join_thread();

    // ASSERT
    // There should be nothing in the VariableServer's thread list
    EXPECT_EQ(varserver->get_vst(id), (Trick::VariableServerThread *) NULL);
    EXPECT_EQ(varserver->get_session(id), (Trick::VariableServerSession *) NULL);
}