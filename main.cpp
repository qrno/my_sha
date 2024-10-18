#include <iostream>
#include <array>
#include <bit>
#include <algorithm>
#include <iomanip>
#include <cstring>
#include <chrono>
#include <cassert>

constexpr uint CH(uint x, uint y, uint z) { return (x & y) ^ ((~x) & z); }
constexpr uint MAJ(uint x, uint y, uint z) { return (x & y) ^ (x & z) ^ (y & z); }
constexpr uint BSIG0(uint x) { return std::rotr(x, 2) ^ std::rotr(x, 13) ^ std::rotr(x, 22); }
constexpr uint BSIG1(uint x) { return std::rotr(x, 6) ^ std::rotr(x, 11) ^ std::rotr(x, 25); }
constexpr uint SSIG0(uint x) { return std::rotr(x, 7) ^ std::rotr(x, 18) ^ (x >> 3); }
constexpr uint SSIG1(uint x) { return std::rotr(x, 17) ^ std::rotr(x, 19) ^ (x >> 10); }

constexpr uint CONCAT(uint a, uint b, uint c, uint d) { return (a << 24) + (b << 16) + (c << 8) + d; }

static constexpr std::array<uint, 64> K{
  0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
  0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
  0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
  0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
  0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
  0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
  0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
  0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
  0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
  0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
  0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
  0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
  0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
  0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
  0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
  0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2,
};

static constexpr std::array<uint, 8> H0{
  0x6a09e667,
  0xbb67ae85,
  0x3c6ef372,
  0xa54ff53a,
  0x510e527f,
  0x9b05688c,
  0x1f83d9ab,
  0x5be0cd19,
};

static std::array<uint, 64> W;
static std::array<uint, 16> M;
static std::array<uint, 8> H;

void perform_hash() {
  std::copy(begin(M), end(M), begin(W));
  for (uint t = 16; t < 64; t++) {
    W[t] = SSIG1(W[t-2]) + W[t-7] + SSIG0(W[t-15]) + W[t-16];
  }

  uint a = H0[0];
  uint b = H0[1];
  uint c = H0[2];
  uint d = H0[3];
  uint e = H0[4];
  uint f = H0[5];
  uint g = H0[6];
  uint h = H0[7];

  for (uint t = 0; t < 64; t++) {
    uint T1 = h + BSIG1(e) + CH(e, f, g) + K[t] + W[t];
    uint T2 = BSIG0(a) + MAJ(a, b, c);

    h = g;
    g = f;
    f = e;
    e = d + T1;
    d = c;
    c = b;
    b = a;
    a = T1 + T2;
  }

  std::fill(begin(H), end(H), 0);
  H[0] += H0[0] + a;
  H[1] += H0[1] + b;
  H[2] += H0[2] + c;
  H[3] += H0[3] + d;
  H[4] += H0[4] + e;
  H[5] += H0[5] + f;
  H[6] += H0[6] + g;
  H[7] += H0[7] + h;
}

unsigned long long best_idx = 0;
static std::array<uint, 8> best;

int main(int argc, char* argv[]) {
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <from> <to>" << std::endl;
    exit(1);
  }

  const unsigned long long from = std::stoull(argv[1]);
  const unsigned long long to = std::stoull(argv[2]);

  if (size(std::to_string(from)) != 13 || size(std::to_string(to)) != 13) {
    std::cerr << "Only 13-digit numbers allowed." << std::endl;
    exit(1);
  }
  if (from > to) {
    std::cerr << "from is more than to?" << std::endl;
    exit(1);
  }

  std::fill(begin(M), end(M), 0);
  std::fill(begin(best), end(best), 0xffffffff);

  M[0] = CONCAT('q','u','i','r');
  M[1] = CONCAT('i','n','o','/');
  M[15] = 21 * 8;

  auto start_time = std::chrono::high_resolution_clock::now();
  unsigned long long hash_count = 0;
  const unsigned long long int STATISTICS_INTERVAL = 4e8;

  for (unsigned long long i = from; i < to; i++) {
    auto S = std::to_string(i);

    M[2] = CONCAT(S[0], S[1], S[2], S[3]);
    M[3] = CONCAT(S[4], S[5], S[6], S[7]);
    M[4] = CONCAT(S[8], S[9], S[10], S[11]);
    M[5] = CONCAT(S[12], 0x80, 0, 0);

    perform_hash();
    hash_count++;

    if (H < best) {
      std::copy(begin(H), end(H), begin(best));
      best_idx = i;
      std::cout << "quirino/" << std::dec << best_idx << '\t';
      for (auto x : H) std::cout << std::hex << std::setw(8) << std::setfill('0') << x << ' ';
      std::cout << std::endl;
    }

    if ((i-from+1) % STATISTICS_INTERVAL == 0) {
      auto current_time = std::chrono::high_resolution_clock::now();
      std::chrono::duration<double> elapsed = current_time - start_time;
      double hashrate = hash_count / elapsed.count();
      double progress = 100.0 * (i - from + 1) / (to - from);

      std::cout << std::dec;
      std::cout << hash_count << "\t" << std::setprecision(3) << std::fixed << hashrate/1e6 << "MH/s\t" << elapsed.count() << "s\t" << progress << "%" << std::endl;
    }
  }

  std::cout << std::endl << std::endl;

  std::cout << "Final Statistics:" << std::endl;
  auto current_time = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = current_time - start_time;
  double hashrate = hash_count / elapsed.count();
  std::cout << std::dec;
  std::cout << hash_count << '\t' << std::setprecision(3) << hashrate/1e6 << "MH/s\t" << elapsed.count() << "s" << std::endl;

  std::cout << "quirino/" << std::dec << best_idx << '\t';
  for (auto x : best) std::cout << std::hex << std::setw(8) << std::setfill('0') << x << ' ';
  std::cout << std::endl;
}
