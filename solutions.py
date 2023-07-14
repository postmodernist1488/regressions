from sympy import simplify, solve, diff
from sympy import Symbol, Eq, Pow, ln, exp
from sympy.abc import a, b, c, x, y

def linear(show_diff=True):
    print('Linear regression:')
    sx2 = Symbol('sx2')
    sx = Symbol('sx')
    sxy = Symbol('sxy')
    sy = Symbol('sy')
    n   = Symbol('n')

    if show_diff:
        error = (a * x + b - y)**2
        diff_a, diff_b = diff(error, a), diff(error, b)
        print('d(ax + b)/da =', diff_a)
        print('d(ax + b)/db =', diff_b)

    equations = [
        a*sx2 + b*sx - sxy,
        a*sx + b*n - sy
    ]
    sol = solve(equations, [a, b])
    # упрощение
    simplified = solve([equations[1], Eq(a, sol[a])], [b])
    sol[b] = simplified[b]

    for key, value in sol.items():
        print(f'{key} = {value}')

def quadratic(show_diff=True):
    print('Quadratic regression:')
    sx4 = Symbol('sx4') 
    sx3 = Symbol('sx3')
    sx2 = Symbol('sx2')
    sx = Symbol('sx')
    sx2y = Symbol('sx2y')
    sxy = Symbol('sxy')
    sy = Symbol('sy')
    n   = Symbol('n')

    if show_diff:
        error = (a * x**2 + b * x + c - y)**2
        diff_a, diff_b, diff_c = diff(error, a), diff(error, b), diff(error, c)
        print('d(ax^2 + bx + c)/da =', diff_a)
        print('d(ax^2 + bx + c)/db =', diff_b)
        print('d(ax^2 + bx + c)/dc =', diff_b)

    equations = [
        a*sx4 + b*sx3 + c*sx2 - sx2y,
        a*sx3 + b*sx2 + c*sx - sxy,
        a*sx2 + b*sx + c*n - sy
    ]
    sol = solve(equations, [a, b, c])
    # подставим а во 2 и 3 уравнения
    simplified = solve([equations[1], equations[2], Eq(a, sol[a])], [b, c])
    sol[b], sol[c] = simplified[b], simplified[c]
    # подставим b в 3 уравнение
    simplified = solve([equations[2], Eq(b, sol[b])], [c])
    sol[c] = simplified[c]

    for key, value in sol.items():
        print(f'{key} = {value}')

def power(show_diff=True):
    print('Power regression:')
    sln2x = Symbol('sln2x')
    slnx = Symbol('slnx')
    slnxlny = Symbol('slnxlny')
    slny = Symbol('slny')
    n   = Symbol('n')
    
    lna = Symbol('lna')

    if show_diff:
        error = (a*Pow(x, b) - y)**2
        diff_a, diff_b = diff(error, a), diff(error, b)
        print('d(ax^b)/da =', diff_a)
        print('d(ax^b)/db =', diff_b)

    equations = [
        b*sln2x + lna*slnx - slnxlny,
        b*slnx + lna*n - slny
    ]
    sol = solve(equations, [lna, b], dict=True)[0]
    simplified = solve([equations[1], Eq(b, sol[b])], [lna], dict=True)[0]
    sol[lna] = simplified[lna]
    print(f'b = {sol[b]}')
    print(f'a = {exp(simplified[lna])}')

def exponential(show_diff=True):
    print('Exponential regression:')
    sx2 = Symbol('sx2')
    sx = Symbol('sx')
    sxlny = Symbol('sxlny')
    slny = Symbol('slny')
    n   = Symbol('n')

    lna = Symbol('lna')
    lnb = Symbol('lnb')

    if show_diff:
        error = (a*Pow(b, x) - y)**2
        diff_a, diff_b = diff(error, a), diff(error, b)
        print('d(ab^x)/da =', diff_a)
        print('d(ab^x)/db =', diff_b)

    equations = [
        lnb*sx2 + lna*sx - sxlny,
        lnb*sx + lna*n - slny
    ]
    sol = solve(equations, [lna, lnb], dict=True)[0]
    simplified = solve([equations[1], Eq(lnb, sol[lnb])], [lna], dict=True)[0]
    print(f'b = {exp(sol[lnb])}')
    print(f'a = {exp(simplified[lna])}')

#TODO: diff + formula
if __name__ == '__main__':
    show_diff = True
    linear(show_diff)
    print()
    quadratic(show_diff)
    print()
    power(show_diff)
    print()
    exponential(show_diff)


