import numpy as np
import matplotlib.pyplot as plt
from scipy.stats import describe

O0 = np.loadtxt('O0.tsv')
O3 = np.loadtxt('O3.tsv')

plt.figure()
(_, minmax, mean, var, _, _) = describe(O0)
plt.bar([r'$AB$, O0', r'$A^T B$, O0', r'$A B^T$, O0'], mean, yerr=np.sqrt(var))
(_, minmax, mean, var, _, _) = describe(O3)
plt.bar([r'$AB$, O3', r'$A^T B$, O3', r'$A B^T$, O3'], mean, yerr=np.sqrt(var))
plt.savefig("p02.png")