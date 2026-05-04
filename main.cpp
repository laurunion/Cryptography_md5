#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <cstring>
#include <cstdint>
using namespace std;

// ─── Step 1: Per-round shift amounts ───────────────────────────────────────
const uint32_t S[64] = {
    7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  // Round 1
    5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  // Round 2
    4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  // Round 3
    6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21   // Round 4
};

// ─── Step 2: Pre-computed table from sin (K[i] = floor(2^32 * |sin(i+1)|)) ─
const uint32_t K[64] = {
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
    0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
    0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
    0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
    0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
    0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
    0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
    0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
    0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
    0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
    0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
};

// ─── Step 3: Left-rotate a 32-bit value ────────────────────────────────────
uint32_t leftRotate(uint32_t x, uint32_t n) {
    return (x << n) | (x >> (32 - n));
}

// ─── Step 4: The four nonlinear functions (one per round) ──────────────────
//   F mixes B, C, D using AND/OR/NOT (Round 1)
//   G is a rotated version of F      (Round 2)
//   H uses XOR (avalanche effect)    (Round 3)
//   I uses XOR + NOT                 (Round 4)
uint32_t F(uint32_t b, uint32_t c, uint32_t d) { return (b & c) | (~b & d); }
uint32_t G(uint32_t b, uint32_t c, uint32_t d) { return (b & d) | (c & ~d); }
uint32_t H(uint32_t b, uint32_t c, uint32_t d) { return b ^ c ^ d; }
uint32_t I(uint32_t b, uint32_t c, uint32_t d) { return c ^ (b | ~d); }

// ─── Main MD5 function ──────────────────────────────────────────────────────
string md5(const string& input) { //512 bit processing block(64 bytes)

    vector<uint8_t> msg;

    // Convert input string into raw bytes (ASCII values)
    for (char ch : input) {
        msg.push_back(static_cast<uint8_t>(ch));
    }

     // Store original length in bits (needed later for MD5 padding rules)
    uint64_t originalBitLen = input.size() * 8;

    cout << "\n--- Padding Demo ---" << endl;

    cout << "1. Input as ASCII bytes: ";
    for (uint8_t b : msg) cout << (int)b << " ";
    cout << endl;

     // Append a single '1' bit (represented as 0x80 = 10000000)
    msg.push_back(0x80);

    cout << "2. Add 0x80 padding marker: ";
    for (uint8_t b : msg) cout << (int)b << " ";
    cout << endl;

    int zeroCount = 0;
    // Add 0 bytes until message length ≡ 56 mod 64
    // (so final 8 bytes can store original length → total = multiple of 64)
    while (msg.size() % 64 != 56) {
        msg.push_back(0x00);
        zeroCount++;
    }

    cout << "3. Add " << zeroCount
         << " zeros so the message reaches 56 bytes" << endl;

    cout << "   Start of message: ";
    for (int i = 0; i < 10 && i < msg.size(); i++) {
        cout << (int)msg[i] << " ";
    }
    cout << "... total = " << msg.size() << " bytes" << endl;

    cout << "4. Append original length: "
         << input.size() << " bytes = " << originalBitLen << " bits" << endl;

         
    cout << "   Length bytes added: ";

    // Append original length as 8 bytes (little-endian order)
    for (int i = 0; i < 8; i++) {
        uint8_t lengthByte = (originalBitLen >> (8 * i)) & 0xFF;
        msg.push_back(lengthByte);
        cout << (int)lengthByte << " ";
    }
    cout << endl;
    //final padded message
    cout << "5. Final padded block:" << endl;

    cout << "   Start: ";
    for (int i = 0; i < 10 && i < msg.size(); i++) {
        cout << (int)msg[i] << " ";
    }

    cout << "... End: ";
    for (int i = msg.size() - 8; i < msg.size(); i++) {
        cout << (int)msg[i] << " ";
    }

    cout << endl;

    cout << "Final padded size: " << msg.size()
         << " bytes = " << msg.size() * 8 << " bits\n" << endl;

    // Phase 2: Initialize MD5 state
    // These are fixed starting values from the MD5 algorithm.
    // They act like the starting "memory" that gets updated as the message is processed.
    uint32_t A = 0x67452301;
    uint32_t B = 0xefcdab89;
    uint32_t C = 0x98badcfe;
    uint32_t D = 0x10325476;

    cout << "\n--- Processing Demo ---" << endl;
    cout << "MD5 now processes the padded message in 64-byte blocks." << endl;
    cout << "Each block is split into 16 words of 32 bits each." << endl;
    cout << "Then MD5 runs 64 mixing steps using F, G, H, and I." << endl;

// Go through the padded message one 64-byte block at a time.
// 64 bytes = 512 bits, which is the block size MD5 uses.
    for (size_t offset = 0; offset < msg.size(); offset += 64) {

        // This array stores the current 64-byte block as 16 smaller 32-bit words.
        uint32_t M[16];

        // Combine every 4 bytes into one 32-bit word.
    // MD5 reads these 4-byte groups in little-endian order.
        for (int j = 0; j < 16; j++) {
            M[j] = (uint32_t)msg[offset + j*4]
                | ((uint32_t)msg[offset + j*4 + 1] << 8)
                | ((uint32_t)msg[offset + j*4 + 2] << 16)
                | ((uint32_t)msg[offset + j*4 + 3] << 24);
        }

        cout << "\nBlock created: 64 bytes → 16 words" << endl;
        cout << "Example: M[0] = " << M[0] << endl;

        // Copy the current hash state into working variables.
    // These get changed during the 64 mixing steps.
        uint32_t a = A, b = B, c = C, d = D;

        // Run 64 total operations.
    // These are split into 4 rounds of 16 operations each.
        for (int i = 0; i < 64; i++) {
            uint32_t f, g;

                    // Round 1: use function F and process message words in normal order.
        if (i < 16) { 
            f = F(b, c, d); 
            g = i; 
        }

        // Round 2: use function G and choose message words in a new order.
        else if (i < 32) { 
            f = G(b, c, d); 
            g = (5*i + 1) % 16; 
        }

        // Round 3: use function H and another message-word order.
        else if (i < 48) { 
            f = H(b, c, d); 
            g = (3*i + 5) % 16; 
        }

        // Round 4: use function I and the final message-word order.
        else { 
            f = I(b, c, d); 
            g = (7*i) % 16; 
        }

                // Save d before rotating the working variables.
            uint32_t temp = d;

              // Rotate the working variables.
        // Each step shifts the roles of a, b, c, and d.
            d = c;
            c = b;
             // Main MD5 mixing step:
        // combine old state + nonlinear function + constant + message word,
        // rotate the bits, then add the result into b.
            b = b + leftRotate(a + f + K[i] + M[g], S[i]);
                    // Old d becomes the new a.
            a = temp;
        }

        // Add the result of this block back into the main hash state.
    // This makes each block affect the final hash.
        A += a;
        B += b;
        C += c;
        D += d;
    }

    cout << "After processing, A, B, C, and D hold the final 128-bit state." << endl;
    // ── Phase 4: Convert final state into hash ───────────────────
    // Each value is 32 bits.
    // A + B + C + D = 128 bits total.
    // Hex prints 4 bits per character, so 128 bits = 32 hex characters.
    auto toHex = [](uint32_t val) -> string {
        stringstream ss;
// Convert each 32-bit value into 4 hex bytes.
    // MD5 outputs these bytes in little-endian order.
        for (int i = 0; i < 4; i++) {
            ss << hex << setw(2) << setfill('0')
            << ((val >> (8*i)) & 0xFF);
        }

        return ss.str();
    };

// Combine A, B, C, and D into the final 32-character MD5 hash.
return toHex(A) + toHex(B) + toHex(C) + toHex(D);
}


int main() {
    string text;
    cout << "Enter text: ";
    getline(cin, text);

    string hash = md5(text);
    cout << "MD5 Hash:   " << hash << endl;
    cout << "Hash length: " << hash.length() << " characters (128 bits)" << endl;

    return 0;
}