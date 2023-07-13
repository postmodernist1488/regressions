from sympy import solve, diff
from sympy import Symbol, symbols, Eq, Pow

def linear():
    sx2 = Symbol('sx2')
    sx = Symbol('sx')
    sxy = Symbol('sxy')
    sy = Symbol('sy')
    n   = Symbol('n')
    a, b = symbols('a,b')

    equations = [
        a*sx2 + b*sx - sxy,
        a*sx + b*n - sy
    ]
    sol = solve(equations, [a, b])
    # упрощение
    simplified = solve([equations[1], Eq(a, sol[a])], [b])
    sol[b] = simplified[b]
    return sol
def quadratic():
    sx4 = Symbol('sx4') 
    sx3 = Symbol('sx3')
    sx2 = Symbol('sx2')
    sx = Symbol('sx')
    sx2y = Symbol('sx2y')
    sxy = Symbol('sxy')
    sy = Symbol('sy')
    n   = Symbol('n')
    a, b, c = symbols('a,b,c')

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
    return sol


if __name__ == '__main__':
    print('Linear regression:')
    linear_sol = linear()
    for key, value in linear_sol.items():
        print(f'{key} = {value}')
    print()

    print('Quadratic regression:')
    quadratic_sol = quadratic()
    for key, value in quadratic_sol.items():
        print(f'{key} = {value}')
    print()

