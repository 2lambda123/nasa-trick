monte_carlo.mc_master.active = True
monte_carlo.mc_master.generate_dispersions = False

exec(open('RUN_file_skip2/input.py').read())
monte_carlo.mc_master.monte_run_number = 0

test.x_file_lookup[0] = 2
test.x_file_lookup[1] = 17
test.x_file_lookup[2] = 33
