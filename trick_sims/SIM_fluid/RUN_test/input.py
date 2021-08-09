import math
exec(open("Modified_data/realtime.py").read())
dyn.fluid.BOUND = 300
dyn.fluid.ISO_RADIUS = 32

mode = -1


if mode == 0:
    # paraboloid initial condition
    for i in range(dyn.fluid.NUM_PARTICLES):
        ROOT_NUM_PARTICLES = math.sqrt(dyn.fluid.NUM_PARTICLES)
        tx = i / ROOT_NUM_PARTICLES - (ROOT_NUM_PARTICLES / 2)
        tz = i % ROOT_NUM_PARTICLES - (ROOT_NUM_PARTICLES / 2)
        dyn.fluid.particlesArr[i].pos[0] = tx * 4
        dyn.fluid.particlesArr[i].pos[2] = tz * 4 
        dyn.fluid.particlesArr[i].pos[1] = tx * tx + tz * tz
if mode == 1:
    # cosine wave initial condition
    for i in range(dyn.fluid.NUM_PARTICLES):
        t = i / 2
        dyn.fluid.particlesArr[i].pos[1] = 15 * math.cos(math.radians(t))



elif mode == 2:
    dyn.fluid.NUM_PARTICLES = 800
    n = dyn.fluid.NUM_PARTICLES
    # concentric circles initial conditions
    for i in range(n):
        theta = (360 / (n / 2)) * i
        print(theta)
        dyn.fluid.particlesArr[i].pos[0] = 100 * math.cos(math.radians(theta))
        dyn.fluid.particlesArr[i].pos[1] = 100 * math.sin(math.radians(theta))
        if i > n / 2 and i <= (3 / 4) * n:
            theta = (360 / ((3 / 8) * n)) * i
            dyn.fluid.particlesArr[i].pos[0] = 70 * math.cos(math.radians(theta))
            dyn.fluid.particlesArr[i].pos[1] = 70 * math.sin(math.radians(theta))
        if i > (7 / 8) * n:
            theta = (360 / ((1 / 8) * n)) * i
            dyn.fluid.particlesArr[i].pos[0] = 40 * math.cos(math.radians(theta))
            dyn.fluid.particlesArr[i].pos[1] = 40 * math.sin(math.radians(theta))
