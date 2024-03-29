/* copyright to Ashar Fuadi [fushar] */

#include <cstdio>
#include <cstring>

#include <vector>
#include <string>
#include <stack>
#include <queue>
#include <map>
#include <set>
#include <iostream>
#include <algorithm>

using namespace std;

#define REP(i,n) for (int i = 0; i < (int)n; i++)
#define FOR(i, a, b) for (int i = (int)a; i <= (int)b; i++)
#define RESET(c,v) memset(c, v, sizeof(c))
#define REPE(i,c) for (typeof((c).end()) i = (c).begin(); i != (c).end(); ++i)


typedef long long ll;

#define pb push_back
#define mp make_pair


struct NFA_node;
struct DFA_node;

int NFA_ID;
map<int, NFA_node*> NFA_map;

int DFA_ID;
map<set<int>, DFA_node*> DFA_map;

struct DFA_node
{
	DFA_node* next[2];
	set<int> states;
	int id;

	DFA_node()
	{
		next[0] = next[1] = NULL;
		id = DFA_ID++;
	}

	static DFA_node* create(set<int> states)
	{
		if (DFA_map.count(states))
			return DFA_map[states];
		DFA_node* o = new DFA_node();
		o->states = states;
		DFA_map[states] = o;
		return o;
	}

	~DFA_node()
	{
		REP(i, 2) if (next[i])
			delete next[i];
	}
};


struct NFA_node
{
	vector<NFA_node*> next[3];
	int id;

	NFA_node()
	{
		NFA_map[NFA_ID] = this;
		id = NFA_ID++;
	}

	~NFA_node()
	{
		REP(i, 3) REP(j, next[i].size()) if (next[i][j])
			delete next[i][j];
	}
};

struct NFA_state
{
	NFA_node* start;
	NFA_node* end;

	NFA_state(NFA_node* st, NFA_node* ed)
	{
		start = st;
		end = ed;
	}
};

int N, L;
string regex;

set<int> eps_closure(set<int> states)
{
	stack<int> st;
	for (set<int>::iterator i = states.begin(); i != states.end(); ++i)
		st.push(*i);
	set<int> res = states;
	while (!st.empty())
	{
		int t = st.top(); st.pop();

		NFA_node* u = NFA_map[t];

		REP(i, u->next[2].size())
		{
			NFA_node* v = u->next[2][i];
			if (!res.count(v->id))
			{
				res.insert(v->id);
				st.push(v->id);
			}
		}
	}
	return res;
}

set<int> move_closure(set<int> states, int c)
{
	queue<pair<int, bool> > st;
	for (set<int>::iterator i = states.begin(); i != states.end(); ++i)
		st.push(make_pair(*i, false));
	set<int> res;
	set<pair<int, bool> > used;
	for (set<int>::iterator i = states.begin(); i != states.end(); ++i)
		used.insert(make_pair(*i, false));

	while (!st.empty())
	{
		int t = st.front().first;
		bool r = st.front().second;
		st.pop();
		NFA_node* u = NFA_map[t];
		REP(i, u->next[2].size())
		{
			NFA_node* v = u->next[2][i];
			if (!used.count(make_pair(v->id, r)))
			{
				used.insert(make_pair(v->id, r));
				st.push(make_pair(v->id, r));
				if (r)
				{
					res.insert(v->id);
				}
			}
		}
		if (r) continue;
		REP(i, u->next[c].size())
		{
			NFA_node* v = u->next[c][i];
			if (!used.count(make_pair(v->id, true)))
			{
				used.insert(make_pair(v->id, true));
				st.push(make_pair(v->id, true));
				res.insert(v->id);
			}
		}
	}
	return res;
}

int K;
typedef vector<vector<ll> > mat;
const ll MOD = 1000000007;

mat mul(mat a, mat b)
{
	mat c = mat(K, vector<ll>(K));
	REP(i, K) REP(j, K) REP(k, K)
	{
		c[i][j] += (a[i][k] * b[k][j]) % MOD;
		c[i][j] %= MOD;
	}
	return c;
}

mat pow(mat a, int p)
{
	if (p == 1)
		return a;
	if (p % 2)
		return mul(a, pow(a, p-1));
	mat x = pow(a, p/2);
	return mul(x, x);
}

int main()
{
	cin >> N;
	REP(n, N)
	{
		NFA_ID = 0;
		NFA_map.clear();
		DFA_ID = 0;
		DFA_map.clear();
		regex = "";
		string s;
		cin >> s >> L;

		// insert concatenation operators
		REP(i, s.size())
		{
			if (i && (s[i] == '(' || s[i] == 'a' || s[i] == 'b') &&
				     (s[i-1] == ')' || s[i-1] == 'a' || s[i-1] == 'b'))
				regex += "^";
			regex += s[i];
		}
        //cout << regex << endl;
		stack<char> ops;
		stack<NFA_state*> states;
		int irvan = 0, kurung = 0;
		REP(i, regex.size())
		{
			if (regex[i] == '(')
				continue;
			else if (regex[i] == 'a' || regex[i] == 'b')
			{
				NFA_node* st = new NFA_node();
				NFA_node* ed = new NFA_node();
				st->next[regex[i]-'a'].push_back(ed);
				NFA_state* state = new NFA_state(st, ed);

				states.push(state);
			}
			else if (regex[i] == ')')
			{
				char op = ops.top();
				ops.pop();
				if (op == '^')
				{
					NFA_state* b = states.top(); states.pop();
					NFA_state* a = states.top(); states.pop();
					a->end->next[2].push_back(b->start);
					NFA_state* state = new NFA_state(a->start, b->end);
					states.push(state);
				}
				else if (op == '|')
				{
					NFA_state* b = states.top(); states.pop();
					NFA_state* a = states.top(); states.pop();
					NFA_node* start = new NFA_node();
					NFA_node* end = new NFA_node();
					start->next[2].push_back(a->start);
					start->next[2].push_back(b->start);
					a->end->next[2].push_back(end);
					b->end->next[2].push_back(end);
					NFA_state* state = new NFA_state(start, end);
					states.push(state);
				}
				else if (op == '*')
				{
					NFA_state* a = states.top(); states.pop();
					NFA_node* start = new NFA_node();
					NFA_node* end = new NFA_node();
					start->next[2].push_back(a->start);
					start->next[2].push_back(end);
					a->end->next[2].push_back(a->start);
					a->end->next[2].push_back(end);
					NFA_state* state = new NFA_state(start, end);
					states.push(state);
				}
			}
			else
				ops.push(regex[i]);
		}

		NFA_node* NFA_start = states.top()->start;
		NFA_node* NFA_end = states.top()->end;
		states.pop();

		set<int> start_set;
		set<int> used;

		start_set.insert(NFA_start->id);

		start_set = eps_closure(start_set);

		DFA_node* DFA_start = DFA_node::create(start_set);
		used.insert(DFA_start->id);

		queue<DFA_node*> q;
		q.push(DFA_start);

		while (!q.empty())
		{
			DFA_node* u = q.front(); q.pop();

			REP(c, 2)
			{
				set<int> lho = move_closure(u->states, c);
				if (lho.empty()) continue;
				DFA_node* v = DFA_node::create(lho);
				u->next[c] = v;
				if (!used.count(v->id))
				{
					used.insert(v->id);
					q.push(v);
				}
			}
		}


//		REPE(a, DFA_map)
//			a->second->trace(NFA_end->id);
//
//		REPE(a, NFA_map)
//			a->second->trace();


		K = DFA_ID;
		mat R(K, vector<ll>(K));
		for (map<set<int>, DFA_node*>::iterator i = DFA_map.begin(); i != DFA_map.end(); ++i)
		{
			DFA_node* u = i->second;
			REP(c, 2)
			{
				DFA_node* v = u->next[c];
				if (v)
				{
					R[v->id][u->id]++;
				}
			}
		}
        for(int i=0;i<R.size();i++)
        {
            for(int j=0;j<R[i].size();j++)
            {
                printf("%d ", R[i][j]);
            }
            printf("\n");
        }
        printf("\n");

		R = pow(R, L);
		ll res = 0;
		for (map<set<int>, DFA_node*>::iterator i = DFA_map.begin(); i != DFA_map.end(); ++i)
			if (i->second->states.count(NFA_end->id))
			res = (res + R[i->second->id][0]) % MOD;

        for(int i=0;i<R.size();i++)
        {
            for(int j=0;j<R[i].size();j++)
            {
                printf("%d ", R[i][j]);
            }
            printf("\n");
        }

		printf("%lld\n", res);
	}
}
