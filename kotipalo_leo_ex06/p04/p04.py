import numpy as np
import matplotlib.pyplot as plt
from scipy.stats import describe
from scipy.optimize import curve_fit

x = np.linspace(0, 1E8, 51)
y = np.zeros(51)
yerr = np.zeros(51)
for i in range(51):
	a = np.loadtxt(f'times_{i}.tsv')
	(_, _, y[i], var, _, _) = describe(a)
	yerr[i] = np.sqrt(var)

(b, l), pcov = curve_fit(lambda x, b, l: x/b + l, xdata=x, ydata=y, sigma=yerr)
(b_err, l_err) = np.sqrt(np.diag(pcov))
print(f'b = {b:e} +- {b_err:e} B/s')
print(f'l = {l} +- {l_err} s')
plt.errorbar(x, y, yerr, fmt=' ', capsize=2.0)
plt.plot(x, x/b + l)
plt.xlabel(f'$N$ (B)')
plt.ylabel(f'$t$ (s)')
plt.savefig('p04.png')