import numpy as np
import matplotlib.pyplot as plt
from scipy.stats import describe

t = np.stack([np.loadtxt(f'times_{i}.tsv') for i in (1, 2, 4, 8)])
(_, _, mean_t, var_t, _, _) = describe(t, axis=1)

plt.bar(['1', '2', '4', '8'], mean_t, yerr=np.sqrt(var_t), capsize=2.0, label='Average times with standard deviation')
plt.xlabel(r'$N_\mathrm{tasks}$')
plt.ylabel(r'$t$')
plt.legend()
plt.show()