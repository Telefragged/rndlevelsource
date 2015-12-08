#pragma once

#define UNIFORMUINT(a, b) std::uniform_int_distribution<unsigned int>(a, b)
#define RNDENG std::mersenne_twister_engine<unsigned int, 32U, 624U, 397U, 31U, 0x9908b0dfU, 11U, 0xffffffffU, 7U, 0x9d2c5680U, 15U, 0xefc60000U, 18U, 1812433253U>
