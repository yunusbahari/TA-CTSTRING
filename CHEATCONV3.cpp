#include <cstdio>
#include <algorithm>
#include <cstring>
#include <ctime>
#include <map>
#include <stack>
#include <vector>

#define MOD 1000000007
#define MNODE 200

using namespace std;

int NID = 0;
int DID = 0;

template <typename T>
struct labelset{
    unsigned long long hash_id;
    T dt[101];
    int data;

	// initialize label without id
	labelset()
    {
        hash_id = 0;
        data = 0;
    }

	// initialize label with id

	// struct comparator with '==' operator
	// if dt equal return true, else return false
    bool operator==(const labelset &o) const
	{
		return hash_id == o.hash_id;
    }

	// struct comparator with '==' operator
    // if |data| < cmp.|data| ret true;
	// if data[i] < cmp.data[i] ret true;
	// else ret false;
	bool operator<(const labelset &o) const
	{
            return hash_id < o.hash_id;
    }

	// insert new element to label
	// check for duplicates and sort the element after insert
	// if exists ret 0, else return 1
    bool push(T ins)
    {
        for(int i=0;i<data;i++)
        {
            if(ins == dt[i])
            {
                return false;
            }
        }
        dt[data++] = ins;
		sort(dt,dt+data);
		return true;
    }

    void CalculateHash()
    {
        hash_id = 0;
        for(int i=0;i<data;i++)
        {
            hash_id=hash_id*211+dt[i];
        }
    }

	// check element in label
	// if exists return 1, else return 0
    bool count(T ins)
    {
        for(int i=0;i<data;i++)
        {
            if(ins == dt[i])
            {
                return true;
            }
        }
        return false;
    }

	// return how much nodes in label
    int size()
    {
        return data;
    }

    void clear()
    {
		hash_id = 0;
        data = 0;
    }

};

struct pairint
{
    int a;
    int b;
    pairint(){ }
    pairint(int _a, int _b)
    {
        a = _a;
        b = _b;
    }
};

stack<pairint> states;
vector<int> nfa_nodes[MNODE][3];
int dfa[MNODE][2];
labelset<int> dfa_label[MNODE];
map<labelset<int>, int> DFA_GRAPH;

void Move(labelset<int> node, int character, labelset<int> &retval)
{
    retval.clear();
    for( int i=0; i<node.data; i++)
    {
        for(int j=0;j<nfa_nodes[node.dt[i]][character].size();j++)
        {
            retval.push(nfa_nodes[node.dt[i]][character][j]);
        }
    }
	retval.CalculateHash();
}

void EpsMove(labelset<int> &retval)
{
    stack<int> nodes;
    int tmp;
    for( int i=0; i < retval.size();i++)
    {
        nodes.push(retval.dt[i]);
    }
    while(nodes.empty() == false)
    {
        int t = nodes.top();
        nodes.pop();
        for(int i=0; i<nfa_nodes[t][2].size(); i++)
        {
            tmp = nfa_nodes[t][2][i];
            if(retval.count(tmp) == 0)
            {
                retval.push(tmp);
                nodes.push(tmp);
            }
        }
    }
	retval.CalculateHash();
}

int AddToGraph(labelset<int> _label)
{
	if(DFA_GRAPH.count(_label) == 1)
		return DFA_GRAPH[_label];

    for(int i=0;i<_label.data;i++)
    {
        dfa_label[DID].push(_label.dt[i]);
    }
    dfa_label[DID].CalculateHash();
    DFA_GRAPH[dfa_label[DID]] = DID;
    DID++;
    return DID-1;
}

void reset()
{
    NID = 0;
    DID = 0;

    memset(dfa, -1, sizeof(dfa));
    memset(dfa_label, 0, sizeof(dfa_label));

    for(int i=0;i<MNODE;i++)
    {
        for(int j=0;j<3;j++)
        {
            nfa_nodes[i][j].clear();
        }
    }
    while(!states.empty())
        states.pop();
    DFA_GRAPH.clear();
}

int main()
{
    char in[121], input[222], op;
    int test, len, N, dv, dv2;
    pairint operan1, operan2, state;

    stack<char> ops;

    for(;scanf("%s", input)!=EOF;)
    {
        reset();
        scanf("%d", &N);
        getchar();
        len = strlen(input);
        for(int i=0;i<len;i++)
        {
            switch(input[i])
            {
            case '(':
                   continue;
                break;
            case 'a':
            case 'b':
                {
                    nfa_nodes[NID][input[i]-'a'].push_back(NID+1);
                    states.push(pairint(NID, NID+1));
                    NID+=2;
                }
                break;
            case ')':
                op = '-';
                if(ops.empty() == false)
                {
                    op = ops.top();
                    ops.pop();
                }
                switch(op)
                {
                case '|':
                    {
                        operan2 = states.top(); states.pop();
                        operan1 = states.top(); states.pop();

                        nfa_nodes[NID][2].push_back(operan1.a);
                        nfa_nodes[NID][2].push_back(operan2.a);
                        nfa_nodes[operan1.b][2].push_back(NID+1);
                        nfa_nodes[operan2.b][2].push_back(NID+1);
                        states.push(pairint(NID, NID+1));
                        NID+=2;
                    }
                    break;
                case '.':
                    {
                        operan2 = states.top(); states.pop();
                        operan1 = states.top(); states.pop();
                        nfa_nodes[operan1.b][2].push_back(operan2.a);
                        states.push(pairint(operan1.a, operan2.b));
                    }
                    break;
                case '*':
                    {
                        operan2 = states.top(); states.pop();
                        nfa_nodes[NID][2].push_back(operan2.a);
                        nfa_nodes[operan2.b][2].push_back(NID);
                        states.push(pairint(NID, NID));
                        NID++;
                    }
                    break;
                }
                break;

            default:
                ops.push(input[i]);
            }
        }
        state = states.top();
        states.pop();

        int start = state.a;
        int finish = state.b;

		labelset<int> visited;
		labelset<int> retval;

		map<labelset<int>, int>::iterator it_dfa;

        retval.push(start);
        EpsMove(retval);
        // Create DFA from NFA
        int initDFA = AddToGraph(retval);
		visited.push(initDFA);
        stack<int> st;
        st.push(initDFA);
        int tmp;
        while(st.empty() == false)
        {
            dv = st.top();
            st.pop();
            for(int i=0; i<2; i++)
            {
                Move((dfa_label[dv]),i, retval);
                EpsMove(retval);
                if(retval.data == 0) continue;
                tmp = AddToGraph(retval);
                dfa[dv][i] = tmp;
                if(visited.count(tmp) == 0)
                {
                    visited.push(tmp);
                    st.push(tmp);
                }
            }
        }
        while(N--)
        {
            int i;
            tmp = initDFA;
            gets(in);
            len = strlen(in);
            for(i=0;i<len;i++)
            {
                tmp = dfa[tmp][in[i]-'a'];
                if(tmp == -1)
                    break;
            }
            if(i!=len || !dfa_label[tmp].count(finish)) printf("N\n");
            else printf("Y\n");
        }
        printf("\n");
    }
    return 0;
}
