test.mc_master.active = True
test.mc_master.generate_dispersions = False

exec(open('RUN_file_sequential/input.py').read())
test.mc_master.monte_run_number = 5

test.x_file_lookup[0] = 12
test.x_file_lookup[1] = 11
test.x_file_lookup[2] = 10
