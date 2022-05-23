import numpy as np
import matplotlib.pyplot as plt
from scipy.stats import sem
from math import pi

ntasks = 4
n_vertices = (10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 200)
l = np.stack(
    list(np.loadtxt(f'out_{ntasks}_{vertices}.tsv', unpack=True)[0] for vertices in n_vertices)
)

times = np.stack(
    list(np.loadtxt(f'times_{ntasks}_{vertices}.tsv', unpack=True) for vertices in n_vertices)
)

min_l = np.minimum(2 * pi, np.min(l, axis=1))
sigma_1 = np.percentile(l, 68.2, interpolation='lower', axis=1)
sigma_2 = np.percentile(l, 95.4, interpolation='lower', axis=1)
sigma_3 = np.percentile(l, 99.7, interpolation='lower', axis=1)

plt.plot(n_vertices, (sigma_1 - min_l) / min_l * 100, 'x')
plt.xlabel(r'$N$')
plt.ylabel(r'$\sigma$ / %')
plt.savefig('sigma_n.png')

plt.figure()
plt.errorbar(n_vertices, np.mean(times, axis=1), np.std(times, ddof=1, axis=1), None, 'x')
plt.xlabel(r'$N$')
plt.ylabel(r'$t$ / s')
plt.yscale('log')
plt.savefig('t_n.png')
