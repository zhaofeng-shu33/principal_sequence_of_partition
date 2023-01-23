
    

def get_psp(n, K, W):
    def merge(C):
        gamma = -1
        for i in C:
            if gamma < W[i]:
                gamma = W[i]
        new_C = []
        for j in C:
            if W[j] < gamma:
                new_C.append(j)
                continue
            if D.get(j) is not None:
                if K[j] == j:
                    continue
                if D.get(K[j]) is None:
                    D[K[j]] = D[j]
                else:
                    D[K[j]].extend(D[j])
                D.pop(j)
            else:
                if D.get(K[j]) is None:
                    D[K[j]] = [j, K[j]]
                else:
                    D[K[j]].append(j)
        P = []
        _total = []
        for D_k in D.values():
            P.append(set(D_k))
            _total.extend(D_k)
        if len(_total) < n:
            div_set = set(range(n)) - set(_total)
            for i in div_set:
                P.append(set([i]))
        return (P, gamma, new_C)
    # n: number of nodes
    Q = [[set([i]) for i in range(n)]]
    L = []
    C = list(range(n))
    D = {}
    while len(C) > 1:
        P, gamma, C = merge(C)
        L.insert(0, gamma)
        Q.insert(0, P)
    return (L, Q)

def test_1():
    n = 4
    K = [0,0,0,0]
    W = [2,5,4,2]
    L_true = [2, 4, 5]
    Q_true = [[{0, 1, 2, 3}], [{0, 1, 2}, {3}], [{0, 1}, {2}, {3}], [{0}, {1}, {2}, {3}]]
    L, Q = get_psp(n, K, W)
    print(L, Q)
    assert(L == L_true)
    assert(Q == Q_true)

def test_2():
    n = 4
    K = [0,0,0,2]
    W = [0.9,1,0.9,2]
    L, Q = get_psp(n, K, W)
    assert(L == [0.9, 1, 2])
    assert(Q == [[{0, 1, 2, 3}], [{2, 3}, {0, 1}], [{2, 3}, {0}, {1}], [{0}, {1}, {2}, {3}]])

test_1()