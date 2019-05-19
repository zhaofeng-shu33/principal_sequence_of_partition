'''Compute edit distance between two ete3 trees
'''
from zss import simple_distance, Node

def _copy_ete_zss(zss_node, ete_node):
    if(ete_node.is_leaf()):
        zss_node.label = ete_node.name
        return
    for child in ete_node.children:
        kid = Node("")
        zss_node.addkid(kid)
        _copy_ete_zss(kid, child)
        
def construct_zss_tree(ete_tree):
    root = Node('')
    _copy_ete_zss(root, ete_tree)
    return root
    
def distance(tree_1, tree_2):
    zss_tree_1 = construct_zss_tree(tree_1)
    zss_tree_2 = construct_zss_tree(tree_2)
    return simple_distance(zss_tree_1, zss_tree_2)
    
if __name__ == '__main__':
    from ete3 import Tree
    a = Tree('(A,(B,C));')
    t = construct_zss_tree(a)
    print(t)