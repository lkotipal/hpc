import numpy as np
import matplotlib.pyplot as plt
from math import pi

n_tasks = (1, 2, 4, 8, 16)
vertices = 100
l = np.stack(
    list(np.loadtxt(f'out_{ntasks}_{vertices}.tsv', unpack=True)[0] for ntasks in n_tasks)
)

times = np.stack(
    list(np.loadtxt(f'times_{ntasks}_{vertices}.tsv', unpack=True) for ntasks in n_tasks)
)

min_l = np.minimum(2 * pi, np.min(l, axis=1))
sigma_1 = np.percentile(l, 68.2, interpolation='lower', axis=1)
sigma_2 = np.percentile(l, 95.4, interpolation='lower', axis=1)
sigma_3 = np.percentile(l, 99.7, interpolation='lower', axis=1)

plt.bar(n_tasks, (sigma_1 - min_l) / min_l * 100)
plt.xlabel(r'Processes')
plt.ylabel(r'$\sigma$ / %')
plt.show()

plt.bar(n_tasks, np.mean(times, axis=1), np.std(times, ddof=1, axis=1))
plt.xlabel('Processes')
plt.ylabel(r'$t$ / s')
plt.show()
