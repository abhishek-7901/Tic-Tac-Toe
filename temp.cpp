#include <bits/stdc++.h>
using namespace std;
using namespace std :: chrono ; 
typedef long long ll;

#define deb(x) cerr << #x << "=" << x << endl
#define deb2(x, y) cerr << #x << "=" << x << "," << #y << "=" << y << endl
#define deb3(x,y,z) cerr << #x << "=" << x << "," << #y << "=" << y << "," << #z << "=" << z<<endl
#define pb push_back
#define fast                          \
    ios_base::sync_with_stdio(false); \
    cin.tie(NULL);                    
#define fo(var, start,end,step) for(var= start ;var<= end ;var+= step)
#define fo1(var, start,end,step) for(var= start ;var>= end ;var-= step) 
#define umap unordered_map
#define uset unordered_set
#define lb lower_bound
#define ub upper_bound
#define all(v) (v).begin(),(v).end()
#define sort0(v) sort(all(v))
typedef pair<int, int> pii;
typedef vector<ll> vll;
typedef vector<bool> vb ; 
typedef pair<ll, ll> pll;
#define sz(x) (ll)x.size()
#define pb push_back
#define ppb pop_back
#define mkp make_pair



#ifndef ONLINE_JUDGE
#define debug(x) cerr << #x <<" "; _print(x); cerr << endl;
#else
#define debug(x)
#endif

typedef unsigned long long ull;
typedef long double lld;

void _print(ll t) {cerr << t;}
void _print(int t) {cerr << t;}
void _print(string t) {cerr << t;}
void _print(char t) {cerr << t;}
void _print(lld t) {cerr << t;}
void _print(double t) {cerr << t;}
void _print(ull t) {cerr << t;}

template <class T, class V> void _print(pair <T, V> p);
template <class T> void _print(vector <T> v);
template <class T> void _print(set <T> v);
template <class T, class V> void _print(map <T, V> v);
template <class T> void _print(multiset <T> v);
template <class T, class V> void _print(pair <T, V> p) {cerr << "{"; _print(p.first); cerr << ","; _print(p.second); cerr << "}";}
template <class T> void _print(vector <T> v) {cerr << "[ "; for (T i : v) {_print(i); cerr << " ";} cerr << "]";}
template <class T> void _print(set <T> v) {cerr << "[ "; for (T i : v) {_print(i); cerr << " ";} cerr << "]";}
template <class T> void _print(multiset <T> v) {cerr << "[ "; for (T i : v) {_print(i); cerr << " ";} cerr << "]";}
template <class T, class V> void _print(map <T, V> v) {cerr << "[ "; for (auto i : v) {_print(i); cerr << " ";} cerr << "]";}

#define inf 1000000000000000005
const ll mod = 1e9+7 ;
 
ll inv(ll i) {if (i == 1) return 1; return (mod - ((mod / i) * inv(mod % i)) % mod) % mod;}
 
ll mod_mul(ll a, ll b) {a = a % mod; b = b % mod; return (((a * b) % mod) + mod) % mod;}
 
ll mod_add(ll a, ll b) {a = a % mod; b = b % mod; return (((a + b) % mod) + mod) % mod;}
 
ll mod_sub(ll a, ll b) {a = a % mod; b = b % mod; return (((a - b + mod) % mod) + mod) % mod;}
 
ll gcd(ll a, ll b) { if (b == 0) return a; return gcd(b, a % b);}
 
ll ceil_div(ll a, ll b) {return ceil(a * 1.0 / b);}
 
ll pwr(ll a, ll b) {a %= mod; ll res = 1; while (b > 0) {if (b & 1) res = res * a % mod; a = a * a % mod; b >>= 1;} return res;}
ll i,j ; 
bool even(ll n) {return n % 2 == 0;}
bool odd(ll n) {return n % 2 == 1;}
bool bit_set(ll n, ll i) {return (n & (1 << i));}
void no(){cout<<"NO"<<endl;}

#define endl "\n" 

void solve(){

}



int main()
{
    auto start = high_resolution_clock::now();
    fast; 
#ifndef ONLINE_JUDGE  
    freopen("error.txt", "w", stderr);
    freopen("input.txt", "r", stdin); 
    freopen("output.txt", "w", stdout); 
#endif
    ll t = 1; 
    cin>> t ; 

    while(t--)
    {
        solve() ;  
    }
    
    auto end = high_resolution_clock::now();
     // use duration cast method
    auto duration = duration_cast<microseconds>(end - start);
 
    cerr << "Time taken by function: "
         << duration.count()/1000 << " milliseconds" << endl;

}