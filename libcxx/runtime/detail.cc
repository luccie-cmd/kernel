#include <hashtable.h>

extern const unsigned long __prime_list[] = // 256 + 1 or 256 + 48 + 1
    {2ul,          3ul,          5ul,          7ul,          11ul,         13ul,
     17ul,         19ul,         23ul,         29ul,         31ul,         37ul,
     41ul,         43ul,         47ul,         53ul,         59ul,         61ul,
     67ul,         71ul,         73ul,         79ul,         83ul,         89ul,
     97ul,         103ul,        109ul,        113ul,        127ul,        137ul,
     139ul,        149ul,        157ul,        167ul,        179ul,        193ul,
     199ul,        211ul,        227ul,        241ul,        257ul,        277ul,
     293ul,        313ul,        337ul,        359ul,        383ul,        409ul,
     439ul,        467ul,        503ul,        541ul,        577ul,        619ul,
     661ul,        709ul,        761ul,        823ul,        887ul,        953ul,
     1031ul,       1109ul,       1193ul,       1289ul,       1381ul,       1493ul,
     1613ul,       1741ul,       1879ul,       2029ul,       2179ul,       2357ul,
     2549ul,       2753ul,       2971ul,       3209ul,       3469ul,       3739ul,
     4027ul,       4349ul,       4703ul,       5087ul,       5503ul,       5953ul,
     6427ul,       6949ul,       7517ul,       8123ul,       8783ul,       9497ul,
     10273ul,      11113ul,      12011ul,      12983ul,      14033ul,      15173ul,
     16411ul,      17749ul,      19183ul,      20753ul,      22447ul,      24281ul,
     26267ul,      28411ul,      30727ul,      33223ul,      35933ul,      38873ul,
     42043ul,      45481ul,      49201ul,      53201ul,      57557ul,      62233ul,
     67307ul,      72817ul,      78779ul,      85229ul,      92203ul,      99733ul,
     107897ul,     116731ul,     126271ul,     136607ul,     147793ul,     159871ul,
     172933ul,     187091ul,     202409ul,     218971ul,     236897ul,     256279ul,
     277261ul,     299951ul,     324503ul,     351061ul,     379787ul,     410857ul,
     444487ul,     480881ul,     520241ul,     562841ul,     608903ul,     658753ul,
     712697ul,     771049ul,     834181ul,     902483ul,     976369ul,     1056323ul,
     1142821ul,    1236397ul,    1337629ul,    1447153ul,    1565659ul,    1693859ul,
     1832561ul,    1982627ul,    2144977ul,    2320627ul,    2510653ul,    2716249ul,
     2938679ul,    3179303ul,    3439651ul,    3721303ul,    4026031ul,    4355707ul,
     4712381ul,    5098259ul,    5515729ul,    5967347ul,    6456007ul,    6984629ul,
     7556579ul,    8175383ul,    8844859ul,    9569143ul,    10352717ul,   11200489ul,
     12117689ul,   13109983ul,   14183539ul,   15345007ul,   16601593ul,   17961079ul,
     19431899ul,   21023161ul,   22744717ul,   24607243ul,   26622317ul,   28802401ul,
     31160981ul,   33712729ul,   36473443ul,   39460231ul,   42691603ul,   46187573ul,
     49969847ul,   54061849ul,   58488943ul,   63278561ul,   68460391ul,   74066549ul,
     80131819ul,   86693767ul,   93793069ul,   101473717ul,  109783337ul,  118773397ul,
     128499677ul,  139022417ul,  150406843ul,  162723577ul,  176048909ul,  190465427ul,
     206062531ul,  222936881ul,  241193053ul,  260944219ul,  282312799ul,  305431229ul,
     330442829ul,  357502601ul,  386778277ul,  418451333ul,  452718089ul,  489790921ul,
     529899637ul,  573292817ul,  620239453ul,  671030513ul,  725980837ul,  785430967ul,
     849749479ul,  919334987ul,  994618837ul,  1076067617ul, 1164186217ul, 1259520799ul,
     1362662261ul, 1474249943ul, 1594975441ul, 1725587117ul, 1866894511ul, 2019773507ul,
     2185171673ul, 2364114217ul, 2557710269ul, 2767159799ul, 2993761039ul, 3238918481ul,
     3504151727ul, 3791104843ul, 4101556399ul, 4294967291ul,
// Sentinel, so we don't have to test the result of lower_bound,
// or, on 64-bit machines, rest of the table.
#if __SIZEOF_LONG__ != 8
     4294967291ul
#else
    6442450933ul, 8589934583ul, 12884901857ul, 17179869143ul,
    25769803693ul, 34359738337ul, 51539607367ul, 68719476731ul,
    103079215087ul, 137438953447ul, 206158430123ul, 274877906899ul,
    412316860387ul, 549755813881ul, 824633720731ul, 1099511627689ul,
    1649267441579ul, 2199023255531ul, 3298534883309ul, 4398046511093ul,
    6597069766607ul, 8796093022151ul, 13194139533241ul, 17592186044399ul,
    26388279066581ul, 35184372088777ul, 52776558133177ul, 70368744177643ul,
    105553116266399ul, 140737488355213ul, 211106232532861ul, 281474976710597ul,
    562949953421231ul, 1125899906842597ul, 2251799813685119ul,
    4503599627370449ul, 9007199254740881ul, 18014398509481951ul,
    36028797018963913ul, 72057594037927931ul, 144115188075855859ul,
    288230376151711717ul, 576460752303423433ul,
    1152921504606846883ul, 2305843009213693951ul,
    4611686018427387847ul, 9223372036854775783ul,
    18446744073709551557ul, 18446744073709551557ul
#endif
};
namespace std::__detail {
std::size_t _Prime_rehash_policy::_M_next_bkt(std::size_t __n) const {
    // Optimize lookups involving the first elements of __prime_list.
    // (useful to speed-up, eg, constructors)
    static const unsigned char __fast_bkt[] = {2, 2, 2, 3, 5, 5, 7, 7, 11, 11, 11, 11, 13, 13};

    if (__n < sizeof(__fast_bkt)) {
        if (__n == 0)
            // Special case on container 1st initialization with 0 bucket count
            // hint. We keep _M_next_resize to 0 to make sure that next time we
            // want to add an element allocation will take place.
            return 1;

        _M_next_resize = __builtin_floor(__fast_bkt[__n] * (double)_M_max_load_factor);
        return __fast_bkt[__n];
    }

    // Number of primes (without sentinel).
    constexpr auto __n_primes = sizeof(__prime_list) / sizeof(unsigned long) - 1;

    // Don't include the last prime in the search, so that anything
    // higher than the second-to-last prime returns a past-the-end
    // iterator that can be dereferenced to get the last prime.
    constexpr auto __last_prime = __prime_list + __n_primes - 1;

    const unsigned long* __next_bkt = std::lower_bound(__prime_list + 6, __last_prime, __n);

    if (__next_bkt == __last_prime)
        // Set next resize to the max value so that we never try to rehash again
        // as we already reach the biggest possible bucket number.
        // Note that it might result in max_load_factor not being respected.
        _M_next_resize = size_t(-1);
    else
        _M_next_resize = __builtin_floor(*__next_bkt * (double)_M_max_load_factor);

    return *__next_bkt;
}
std::pair<bool, std::size_t> _Prime_rehash_policy::_M_need_rehash(std::size_t __n_bkt,
                                                                  std::size_t __n_elt,
                                                                  std::size_t __n_ins) const {
    if (__n_elt + __n_ins > _M_next_resize) {
        // If _M_next_resize is 0 it means that we have nothing allocated so
        // far and that we start inserting elements. In this case we start
        // with an initial bucket size of 11.
        double __min_bkts = std::max<std::size_t>(__n_elt + __n_ins, _M_next_resize ? 0 : 11) /
                            (double)_M_max_load_factor;
        if (__min_bkts >= __n_bkt)
            return {true, _M_next_bkt(std::max<std::size_t>(__builtin_floor(__min_bkts) + 1,
                                                            __n_bkt * _S_growth_factor))};

        _M_next_resize = __builtin_floor(__n_bkt * (double)_M_max_load_factor);
        return {false, 0};
    } else
        return {false, 0};
}
} // namespace std::__detail
