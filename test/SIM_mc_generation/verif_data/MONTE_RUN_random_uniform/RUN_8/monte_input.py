test.mc_master.active = True
test.mc_master.generate_dispersions = False

exec(open('RUN_random_uniform/input.py').read())
test.mc_master.monte_run_number = 8

test.x_uniform = 58483.47941203944
test.x_integer = 887
