import numpy as np
import matplotlib.pyplot as plt
from scipy.stats import describe

a0 = np.loadtxt('a_O0.tsv')
a3 = np.loadtxt('a_O3.tsv')
b0 = np.loadtxt('b_O0.tsv')
b3 = np.loadtxt('b_O3.tsv')

plt.figure()
(_, minmax, mean, var, _, _) = describe(a0)
plt.bar(['unoptimized, O0', 'optimized, O0'], mean, yerr=np.sqrt(var))
(_, minmax, mean, var, _, _) = describe(a3)
plt.bar(['unoptimized, O3', 'optimized, O3'], mean, yerr=np.sqrt(var))
plt.savefig("p01_a.png")

plt.figure()
(_, minmax, mean, var, _, _) = describe(b0)
plt.bar(['unoptimized, O0', 'optimized, O0'], mean, yerr=np.sqrt(var))
(_, minmax, mean, var, _, _) = describe(b3)
plt.bar(['unoptimized, O3', 'optimized, O3'], mean, yerr=np.sqrt(var))
plt.savefig("p01_b.png")