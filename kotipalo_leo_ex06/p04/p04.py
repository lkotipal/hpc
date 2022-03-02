import numpy as np
import matplotlib.pyplot as plt
from scipy.stats import describe
from scipy.optimize import curve_fit

print(np.linspace(0, 1E8, 51))

x = np.array([2**i for i in range(29)])
y = np.zeros(29)
yerr = np.zeros(29)
for i in range(29):
	a = np.loadtxt(f'times_{i}.tsv')
	(_, _, y[i], var, _, _) = describe(a)
	yerr[i] = np.sqrt(var)

(a, b), _ = curve_fit(lambda x, a, b: a * x + b, xdata=x, ydata=y, sigma=yerr)
plt.errorbar(x, y, yerr, fmt=' ')
plt.plot(x, a * x + b)
plt.show()

#for i in range(8):
#	a = np.loadtxt(f'eigendouble_{x[i]}.tsv')
#	(_, _, y[i], var, _, _) = describe(a)
#	yerr[i] = np.sqrt(var)
#
#(a, b), _ = curve_fit(lambda x, a, b: a * x**b, xdata=x, ydata=y, sigma=yerr)
#
#print(f'a = {a}')
#print(f'b = {b}')
#
#plt.errorbar(x, y, yerr, fmt=' ', capsize=2.0, label='Data')
#plt.plot(x, a * x**b, label=f'$a \cdot N^b$')
#plt.legend()
#plt.xscale('log')
#plt.yscale('log')
#plt.xlabel('N')
#plt.ylabel('t')
#plt.savefig('p01.png')