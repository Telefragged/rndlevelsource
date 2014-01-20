#pragma once

#include "BigInt\BigUnsigned.h"
#include "BigInt\BigIntegerUtils.h"
#include "timer.h"
#include "linkedList.h"
#include "defines.h"
#include <sstream>
#include <thread>
#include <random>

BigUnsigned bigpow(BigUnsigned base, BigUnsigned power) {
	BigUnsigned ret = base;
	for(BigUnsigned n = 1U; n < power; n++) {
		ret = ret * base;
	}
	return ret;
}

BigUnsigned powmod(BigUnsigned a, BigUnsigned x, BigUnsigned n) {
	//mstimer timer("ms powmod()");
	BigUnsigned r(1);
	while(x > 0) {
		if( (x & 1) == 1) r = a*r % n;
		x >>= 1;
		a = a*a % n;
	}
	return r;
}

template <class _randeng>
BigUnsigned bigrand(BigUnsigned min, BigUnsigned max, _randeng &rand) {
	BigUnsigned maxB(max.bitLength()), diff(max-min), res(0);
	const unsigned int bitres = 4;
	std::uniform_int_distribution<unsigned int> bitdist(bitres, maxB.toUnsignedInt());
	std::uniform_int_distribution<unsigned int> numdist(0, (2<<bitres)-1);
	BigUnsigned numBits(bitdist(rand));
	for(BigUnsigned n(0); n < numBits; n+=bitres) {
		res += BigUnsigned(numdist(rand)) << n.toUnsignedInt();
	}
	return BigUnsigned((res % diff) + min);
}

template <class _randeng>
BigUnsigned bigrand(unsigned int numbits, BigUnsigned reserve, _randeng &rand) {
	//mstimer timer("ms bigrand()");
	const unsigned int bitres = 8;
	std::uniform_int_distribution<unsigned int> numdist(0, (2<<bitres)-1);
	BigUnsigned res(0);
	for(unsigned int n = 0; n < numbits; n+=bitres) {
		res += BigUnsigned(numdist(rand)) << n;
	}
	while(res.bitLength() > numbits) res.setBit(res.bitLength()-1, false);
	res.setBit(numbits-1, true);
	if((res + reserve).bitLength() > numbits) res-= reserve;
	return res;
}

bool probable_prime_MR (BigUnsigned n, BigUnsigned k) {;
	//mstimer timer("ms probable_prime_MR()");
	if(n == 2 || n == 3) return true;
	if(n <= 1 || !(n & 1).toUnsignedInt()) return false;
	BigUnsigned s(0U);
	for (BigUnsigned m = n-1; !(m & 1U).toUnsignedInt(); ++s, m >>= 1U);
	BigUnsigned d = (n-1) / (BigUnsigned(1U)<<s.toUnsignedInt());
	std::random_device eng;
	//eng.seed((unsigned int)std::chrono::high_resolution_clock::to_time_t(std::chrono::high_resolution_clock::now()));
	for(BigUnsigned i(0); i < k; i++) {
		BigUnsigned a = bigrand(BigUnsigned(2), n-1, eng);
		BigUnsigned x = powmod(a, d, n);
		if( x == 1 || x == n-1) continue;
		for(BigUnsigned r(0); r < s-1; r++) {
			x = powmod(x, 2, n);
			if(x == 1) return false;
			if(x == n-1) goto LOOP;
		}
		return false;
LOOP:
		continue;
	}
	return true;
}

void sieve(linkedList<bool> &list, BigUnsigned x, BigUnsigned startRange) {
	BigUnsigned n = x*x;
	if(n < startRange) n = startRange - (startRange % x);
	for(; n < startRange + list.size(); n += x) {
		if(n < startRange) continue;
		list.replace(new bool(false), (n - startRange).toUnsignedInt());
	}
}

void sieveAll(linkedList<bool> &list, linkedList<BigUnsigned> &slist, BigUnsigned startRange) {
	for(unsigned int it = 0; it < slist.size(); it++) {
		BigUnsigned x = slist.get(it);
		sieve(list, x, startRange);
	}
}

void findprimes(linkedList<BigUnsigned> &primes, BigUnsigned limit) {
	linkedList<bool> sievelist;
	for(BigUnsigned n(2); n < limit; n++) {
		sievelist.put(new bool(true));
	}
	for(unsigned int n = 2; n < sievelist.size(); n++) {
		if(!sievelist.get(n-2)) continue;
		primes.put(new BigUnsigned(n));
		sieve(sievelist, BigUnsigned(n), BigUnsigned(2));
	}
}

void findrandomprime(linkedList<BigUnsigned> *sievedlist, linkedList<BigUnsigned> *reslist, unsigned int accuracy) {
	mstimer timer;
	timer.start();
	BigUnsigned v(0);
	std::random_device eng;
	//eng.seed((unsigned int)std::chrono::high_resolution_clock::to_time_t(std::chrono::high_resolution_clock::now()));
	bool found = false;
	unsigned int startsize = sievedlist->size();
	std::cout<<startsize<<" elements to search through...\n";
	do {
		v = sievedlist->removeval(UNIFORMUINT(0, sievedlist->size()-1)(eng));
		found = probable_prime_MR(v, accuracy);
	} while (!found && sievedlist->size() > 0 && reslist->size() < 1);
	if(found) reslist->put(new BigUnsigned(v));
	timer.stop();
	std::cout<<(double(startsize-sievedlist->size())/timer.getTime().count()) * 1000.0<<" miller-rabin tests/s k = "<<accuracy
		<<" ("<<startsize-sievedlist->size()<<" test(s) over "<<double(timer.getTime().count()) / 1000.0 <<"s)\n";
}

BigUnsigned generateprime(unsigned int bits = 128U, unsigned int numthreads = std::thread::hardware_concurrency(), unsigned int accuracy = 16u, linkedList<BigUnsigned> &primes = linkedList<BigUnsigned>()) {
	std::ostringstream os;
	os<<"ms generateprime(bits = "<<bits<<", numthreads = "<<numthreads<<")";
	mstimer timer(os.str());
	if(primes.size() == 0) {
		findprimes(primes, stringToBigUnsigned("15000"));
	}
	BigUnsigned reserved = stringToBigUnsigned("200");
	//std::mt19937 eng;
	std::random_device dev;
	//eng.seed(dev());
	//eng.seed((unsigned int)std::chrono::high_resolution_clock::to_time_t(std::chrono::high_resolution_clock::now()));
	BigUnsigned startrange = bigrand(bits, reserved, dev);
	linkedList<bool> largelist;
	for(BigUnsigned i(0); i < reserved; i++) {
		largelist.put(new bool(true));
	}
	sieveAll(largelist, primes, startrange);
	linkedList<BigUnsigned> *sievedlist = new linkedList<BigUnsigned>[numthreads];
	linkedList<BigUnsigned> probprimes;
	for(unsigned int n = 0; n < largelist.size(); n++) {
		std::uniform_int_distribution<unsigned int> dist(0, numthreads-1);
		if(largelist.get(n)) {
			sievedlist[dist(dev)].put(new BigUnsigned(startrange + n));
		}
	}
	std::thread *thr = new std::thread[numthreads];
	for(unsigned int n = 0; n < numthreads; n++) thr[n] = std::thread(findrandomprime, &sievedlist[n], &probprimes, accuracy);
	for(unsigned int n = 0; n < numthreads; n++) thr[n].join();
	unsigned int pickprime = UNIFORMUINT(0, probprimes.size()-1)(dev);
	return probprimes.removeval(pickprime);
}