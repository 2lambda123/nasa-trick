test.mc_master.active = True
test.mc_master.generate_dispersions = False

exec(open('RUN_random_uniform/input.py').read())
test.mc_master.monte_run_number = 5

test.x_uniform = 74245.08441568459
test.x_integer = 18377
