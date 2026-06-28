#include <iostream>
#include <cmath>
#include <iomanip>
#include <cstdlib>
#include <ctime>

const float PI = 3.14159265f;

// 1. THE PERFECT, SYSTEMATIC G-MATRIX (Zero Leakage)
const float G_MATRIX[6][4] = {
    { 1,  0,  0,  0}, // m[0]
    { 0,  1,  0,  0}, // m[1]
    { 0,  0,  1,  0}, // m[2]
    { 0,  0,  0,  1}, // m[3]
    { 1,  1,  0, -1}, // Parity 1
    {-1,  0,  1,  1}  // Parity 2
};

// 2. THE EXACT 1st ROW OF THE 6-POINT DFT (k=1)
const float H_REAL[6] = { 1.0,  0.5, -0.5, -1.0, -0.5,  0.5};
const float H_IMAG[6] = { 0.0, -0.866025f, -0.866025f,  0.0,  0.866025f,  0.866025f};

int main() {
    srand(time(0));

    std::cout << "=========================================\n";
    std::cout << " SYSTEMATIC BCH AUDIO CORRECTOR (V2)\n";
    std::cout << "=========================================\n\n";

    // 1. THE MICROPHONE
    float m[4] = {0.25, -0.80, 0.60, -0.15};
    std::cout << "[INFO] Original Audio Samples: ";
    for(int i=0; i<4; i++) std::cout << m[i] << "  ";
    std::cout << "\n\n";

    // 2. ENCODE
    float y[6] = {0};
    for(int i=0; i<6; i++) {
        for(int j=0; j<4; j++) {
            y[i] += G_MATRIX[i][j] * m[j];
        }
    }

    // 3. THE SABOTAGE
    int error_index = rand() % 6;

    // Let's randomly make the spike positive or negative to prove it handles both
    float error_magnitude = (rand() % 2 == 0) ? 15000.0f : -15000.0f;
    y[error_index] += error_magnitude;

    std::cout << "[!] IMPULSE ERROR INJECTED (" << error_magnitude << ")\n";
    std::cout << "    Corrupted Array Received: \n    [ ";
    for(int i=0; i<6; i++) {
        if(i == error_index) std::cout << "\033[1;31m" << std::fixed << std::setprecision(2) << y[i] << "\033[0m  ";
        else std::cout << std::fixed << std::setprecision(2) << y[i] << "  ";
    }
    std::cout << "]\n\n";

    // 4. THE SYNDROME (Only S1 is needed)
    float S1_real = 0, S1_imag = 0;
    for(int i=0; i<6; i++) {
        S1_real += H_REAL[i] * y[i];
        S1_imag += H_IMAG[i] * y[i];
    }

    // 5. THE LOCATOR & ALIAS RESOLVER
    float S1_mag = sqrt(S1_real*S1_real + S1_imag*S1_imag);

    if (S1_mag > 100.0) {
        std::cout << "[+] BCH Hardware Locator Engaged...\n";

        // Extract phase and map to initial index
        float angle = atan2(S1_imag, S1_real);
        int loc_base = round(-angle / (PI / 3.0f));
        if (loc_base < 0) loc_base += 6;
        loc_base = loc_base % 6;

        // The Ambiguity Resolver: A positive spike at 'loc' looks identical to a negative spike at 'loc+3'.
        // We look at the actual corrupted array. The true error location is the one with the massive absolute value.
        int loc_alias = (loc_base + 3) % 6;
        int true_loc = 0;
        float true_mag = 0;

        if (std::abs(y[loc_base]) > std::abs(y[loc_alias])) {
            true_loc = loc_base;
            true_mag = S1_mag; // It was a positive spike
        } else {
            true_loc = loc_alias;
            true_mag = -S1_mag; // It was a negative spike
        }

        std::cout << "[+] Error Identified at Index : " << true_loc << "\n";
        std::cout << "[+] Magnitude Calculated      : " << true_mag << "\n";

        // Erase it perfectly
        y[true_loc] -= true_mag;
        std::cout << "[✓] Error Erased from Array.\n\n";
    }

    // 6. DECODE
    // Because G is perfectly systematic, the clean audio is already sitting in indices 0 through 3.
    float m_hat[4] = {y[0], y[1], y[2], y[3]};

    // 7. PROOF
    std::cout << "[INFO] Reconstructed Audio Samples: ";
    for(int i=0; i<4; i++) std::cout << std::fixed << std::setprecision(2) << m_hat[i] << "  ";
    std::cout << "\n=========================================\n";

    return 0;
}
