
    

def get_psp(n, K, W):
    def merge(C):
        gamma = -1
        for i in C:
            if gamma < W[i]:
                gamma = W[i]
        new_C = []
        for j in C:
            if W[j] < gamma:
                if D.get(j) is None:
                    new_C.append(j)
                continue
            if j > n:
                if D.get(K[j]) is None:
                    D[K[j]] = D[j]
                else:
                    D[K[j]].extend(D[j])
                D.pop(j)
            else:
                if D.get(K[j]) is None:
                    D[K[j]] = [j]
                else:
                    D[K[j]].append(j)
        new_C.extend(list(D.keys()))
        P = []
        _total = []
        for D_k in D.values():
            P.append(set(D_k))
            _total.extend(D_k)
        if len(_total) < n:
            div_set = set(range(1, n + 1)) - set(_total)
            for i in div_set:
                P.append(set([i]))
        return (P, gamma, new_C)
    # n: number of nodes
    Q = [[set([i]) for i in range(1, n + 1)]]
    L = []
    C = list(range(1, n + 1))
    D = {}
    while len(C) > 1:
        P, gamma, C = merge(C)
        L.insert(0, gamma)
        Q.insert(0, P)
    return (L, Q)

def test_1():
    n = 4
    K = [-1, 5, 5, 6, 0, 6, 0]
    W = [0, 5, 5, 4, 2, 4, 2]
    L_true = [2, 4, 5]
    Q_true = [[{1, 2, 3, 4}], [{1, 2, 3}, {4}], [{1, 2}, {3}, {4}], [{1}, {2}, {3}, {4}]]
    L, Q = get_psp(n, K, W)
    assert(L == L_true)
    assert(Q == Q_true)

def test_2():
    n = 4
    K = [-1, 5, 5, 6, 6, 0, 0]
    W = [0, 1, 1, 2, 2, 0.9, 0.9]
    L, Q = get_psp(n, K, W)
    assert(L == [0.9, 1, 2])
    assert(Q == [[{1, 2, 3, 4}], [{3, 4}, {1, 2}], [{3, 4}, {1}, {2}], [{1}, {2}, {3}, {4}]])

