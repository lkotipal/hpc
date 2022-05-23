import numpy as np
import matplotlib.pyplot as plt
from math import pi

ntasks = 4
l = np.stack(
    list(np.loadtxt(f'out_{ntasks}_{vertices}.tsv', unpack=True)[0] for vertices in (10, 20, 30, 40, 50, 60, 70, 80, 90, 100))
)

min_l = np.minimum(2 * pi, np.min(l, axis=1))
sigma_1 = np.percentile(l, 68.2, interpolation='lower')

plt.plot()

#for vertices in (10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 200):
#    print(f'Problem size n = {vertices}')
#    times = np.loadtxt(f'times_{ntasks}_{vertices}.tsv')
#    l = np.loadtxt(f'out_{ntasks}_{vertices}.tsv', unpack=True)[0]
#    best_l = np.min(l)
#
#    # If best route found is below the upper bound of 2pi, assume it's the actual best route
#    # Otherwise use the upper bound
#    min_l = min(2 * pi, best_l)
#    sigma_1 = np.percentile(l, 68.2, interpolation='lower')
#    sigma_2 = np.percentile(l, 95.4, interpolation='lower')
#    sigma_3 = np.percentile(l, 99.7, interpolation='lower')
#    print(f'Best path found was length {best_l}, using {min_l} as optimal path length:')
#    print(f'Solutions found in {np.mean(times)} +- {np.std(times, ddof=1)} seconds')
#    print(f'sigma-1 of data {sigma_1} for error ~{(sigma_1 - min_l) / min_l * 100}%')
#    print(f'sigma-2 of data {sigma_2} for error ~{(sigma_2 - min_l) / min_l * 100}%')
#    print(f'sigma-3 of data {sigma_3} for error ~{(sigma_3 - min_l) / min_l * 100}%')
#    print()
    
#vertices = 100
#print(f'ntasks = {ntasks}')
#l = np.stack(
#    (np.loadtxt(f'out_{ntasks}_{vertices}.tsv', unpack=True)[0]) for ntasks in (1, 2, 4, 8, 16)
#)
#best_l = np.min(l)
#min_l = min(2 * pi, best_l)

#for i in ('square', 'circle', 'cities'):
#    l = np.loadtxt(f'{i}_lengths.tsv', skiprows=1, unpack=True)[0]
#    if (i == 'square'):
#        print(f'With {np.size(i)} trials:')
#    print(f'Shortest path in {i} {np.min(l)} with {np.sum(l == np.min(l))} hits')
#
#    path = np.loadtxt(f'{i}_route.tsv', unpack=True)
#    x = path[0]
#    y = path[1]
#    plt.figure()
#    plt.plot(np.append(x, x[0]), np.append(y, y[0]), label='Route')
#    plt.xlabel(r'$x$')
#    plt.ylabel(r'$y$')
#    plt.legend()
#    plt.savefig(f'{i}.png')
