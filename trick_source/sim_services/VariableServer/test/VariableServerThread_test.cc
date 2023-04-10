/******************************TRICK HEADER*************************************
PURPOSE:                     ( Tests for the VariableServerThread class )
*******************************************************************************/

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <stdexcept>

#include "trick/VariableServer.hh"
#include "trick/RealtimeSync.hh"
#include "trick/GetTimeOfDayClock.hh"
#include "trick/ITimer.hh"
#include "trick/ExecutiveException.hh"

#include "trick/VariableServerThread.hh"

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

    ON_CALL(*session, copy_data_async())
        .WillByDefault(Return(0));

    ON_CALL(*session, get_pause())
        .WillByDefault(Return(false));
        
    ON_CALL(Const(*session), get_update_rate())
        .WillByDefault(Return(0.001));

    ON_CALL(*session, get_exit_cmd())
        .WillByDefault(Return(false));
}

/*
 Test Fixture.
 */
class VariableServerThread_test : public ::testing::Test {
	protected:
        // Static global dependencies that I would like to eventually mock out
        Trick::VariableServer * varserver;
        Trick::RealtimeSync * realtime_sync;
        Trick::GetTimeOfDayClock clock;
        Trick::ITimer timer;

        MockClientConnection connection;
        NiceMock<MockVariableServerSession> * session;

		VariableServerThread_test() { 
            // Set up dependencies that haven't been broken
            varserver = new Trick::VariableServer;
            Trick::VariableServerThread::set_vs_ptr(varserver);
            realtime_sync = new Trick::RealtimeSync(&clock, &timer);

            // Set up mocks
            session = new  NiceMock<MockVariableServerSession>;
            setup_default_connection_mocks(&connection);
            setup_default_session_mocks(session);
        }

		~VariableServerThread_test() { 
            delete varserver; 
            delete realtime_sync;
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

    // Starting the connection fails
    EXPECT_CALL(connection, start())
        .Times(1)
        .WillOnce(Return(1));
    
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

    delete session;
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
    EXPECT_CALL(*session, copy_data_async())
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


TEST_F(VariableServerThread_test, turn_session_log_on) {
    // ARRANGE
    setup_normal_connection_expectations(&connection);
    set_session_exit_after_some_loops(session);

    varserver->set_var_server_log_on();

    // We expect a the session's log to be turned on
    EXPECT_CALL(*session, set_log_on())
        .Times(1);

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

TEST_F(VariableServerThread_test, throw_trick_executive_exception) {
    // ARRANGE
    setup_normal_connection_expectations(&connection);

    EXPECT_CALL(*session, get_exit_cmd())
        .WillRepeatedly(Return(false));

    // Set up VariableServerThread
    Trick::VariableServerThread * vst = new Trick::VariableServerThread(session) ;
    vst->set_connection(&connection);

    EXPECT_CALL(*session, handle_message())
        .WillOnce(Throw(Trick::ExecutiveException(-1, __FILE__, __LINE__, "Trick::ExecutiveException Error message for testing")));

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

TEST_F(VariableServerThread_test, throw_exception) {
    // ARRANGE
    setup_normal_connection_expectations(&connection);

    EXPECT_CALL(*session, get_exit_cmd())
        .WillRepeatedly(Return(false));

    // Set up VariableServerThread
    Trick::VariableServerThread * vst = new Trick::VariableServerThread(session) ;
    vst->set_connection(&connection);

    EXPECT_CALL(*session, handle_message())
        .WillOnce(Throw(std::logic_error("Error message for testing")));

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