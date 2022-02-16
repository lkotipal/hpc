import numpy as np
import matplotlib.pyplot as plt
from scipy.stats import describe

a1 = np.loadtxt('novectorize_1.tsv', unpack=True)[1]
a2 = np.loadtxt('novectorize_10.tsv', unpack=True)[1]
a3 = np.loadtxt('novectorize_100.tsv', unpack=True)[1]
a4 = np.loadtxt('novectorize_1000.tsv', unpack=True)[1]
a = np.stack((a1, a2, a3, a4))

b1 = np.loadtxt('vectorize_1.tsv', unpack=True)[1]
b2 = np.loadtxt('vectorize_10.tsv', unpack=True)[1]
b3 = np.loadtxt('vectorize_100.tsv', unpack=True)[1]
b4 = np.loadtxt('vectorize_1000.tsv', unpack=True)[1]
b = np.stack((b1, b2, b3, b4))

(_, minmax, mean, var, _, _) = describe(a, axis=1)
plt.errorbar([1, 10, 100, 1000], mean, np.sqrt(var))
(_, minmax, mean, var, _, _) = describe(b, axis=1)
plt.errorbar([1, 10, 100, 1000], mean, np.sqrt(var))
plt.show()