import numpy as np
import matplotlib.pyplot as plt
from scipy.stats import describe

a = np.loadtxt('out_01.tsv')

plt.figure()
(_, minmax, mean, var, _, _) = describe(a)
plt.bar(['formatted', 'unformatted'], mean, yerr=np.sqrt(var))
plt.ylabel(r'$t (s)$')
plt.savefig("p01.png")