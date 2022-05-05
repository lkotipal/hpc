import numpy as np
import matplotlib.pyplot as plt

l_cities = np.loadtxt('cities_lengths.tsv')
for i in ('square', 'circle', 'cities'):
    l = np.loadtxt(f'{i}_lengths.tsv')
    if (i == 'square'):
        print(f'With {np.size(i)} trials:')
    print(f'Shortest path in {i} {np.min(l)} with {np.sum(l == np.min(l))} hits')

    path = np.loadtxt(f'{i}_route.tsv', unpack=True)
    x = path[0]
    y = path[1]
    plt.figure()
    plt.plot(np.append(x, x[0]), np.append(y, y[0]), label='Route')
    plt.xlabel(r'$x$')
    plt.ylabel(r'$y$')
    plt.legend()
    plt.savefig(f'{i}.png')
