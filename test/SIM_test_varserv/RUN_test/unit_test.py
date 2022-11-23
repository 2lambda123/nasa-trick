import trick
import socket

from trick.unit_test import *

def main():

	trick.var_server_set_port(40000)
	trick.var_ascii()
	trick.real_time_enable()
	trick.exec_set_software_frame(0.01)
	# trick.set_var_server_info_msg_on()

	trick.exec_set_terminate_time(5.0)

	varServerPort = trick.var_server_get_port()
	test_output = ( os.getenv("TRICK_HOME") + "/trick_test/SIM_test_varserv.xml" ) 

	# Start the test client
	os.system("./models/test_client/test_client " + str(varServerPort) + " --gtest_output=xml:" + test_output + " &")


if __name__ == "__main__":
	main()

