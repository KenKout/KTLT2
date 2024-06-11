
#include "study_in_pink1.h"

bool readFile(
        const string &filename,
        int &HP1,
        int &HP2,
        int &EXP1,
        int &EXP2,
        int &M1,
        int &M2,
        int &E1,
        int &E2,
        int &E3
) {
    // This function is used to read the input file,
    // DO NOT MODIFY THIS FUNTION
    ifstream ifs(filename);
    if (ifs.is_open()) {
        ifs >> HP1 >> HP2
            >> EXP1 >> EXP2
            >> M1 >> M2
            >> E1 >> E2 >> E3;
        return true;
    } else {
        cerr << "The file is not found" << endl;
        return false;
    }
}

////////////////////////////////////////////////////////////////////////
/// STUDENT'S ANSWER BEGINS HERE
/// Complete the following functions
/// DO NOT modify any parameters in the functions.
////////////////////////////////////////////////////////////////////////

// Task 1
int clampEXP(int points) {
    return max(0, min(points, 600));
}

int firstMeet(int& EXP1, int& EXP2, int E1) {
    // Check if E1 is out of the range [0, 99].
    if (E1 < 0 || E1 > 99) return -99;
    EXP1 = clampEXP(EXP1);
    EXP2 = clampEXP(EXP2);
    // Case 1: E1 is in [0, 3]
    if (E1 >= 0 && E1 <= 3) {
        int D = E1 * 3 + EXP1 * 7; // Calculate decision value
        // Apply changes to Watson's EXP based on the observation outcomes.
        switch (E1) {
            case 0: EXP2 += 29; break;
            case 1: EXP2 += 45; break;
            case 2: EXP2 += 75; break;
            case 3: EXP2 += 29 + 45 + 75; break;
        }
        EXP1 = clampEXP(EXP1);
        EXP2 = clampEXP(EXP2);
        // Apply changes to Sherlock's EXP based on decision value D.
        if (D % 2 == 0) { // D is even
            EXP1 = ceil(EXP1 + static_cast<double>(D) / 200.0);
            EXP1 = clampEXP(EXP1);
        } else { // D is odd
            EXP1 = ceil(EXP1 - static_cast<double>(D) / 100.0);
            EXP1 = clampEXP(EXP1);
        }
        return EXP1 + EXP2;

    }

    // Case 2: E1 is in [4, 99]

    // Information 1
    if (E1 >= 4 && E1 <= 19) {
        EXP2 = ceil(EXP2 + (E1 / 4.0 + 19));
        EXP2 = clampEXP(EXP2);

        // Bound Sherlock's EXP within [0, 600] after Watson's correction
        EXP1 -= E1;
        EXP1 = clampEXP(EXP1);
    }

    // Information 2
    if (E1 >= 20 && E1 <= 49) {
        EXP2 = ceil(EXP2 + (E1 / 9.0 + 21));
        EXP2 = clampEXP(EXP2);

        // Bound Sherlock's EXP within [0, 600] after Watson's correction
        EXP1 -= E1;
        EXP1 = clampEXP(EXP1);
    }
    // Information 3
    if (E1 >= 50 && E1 <= 65) {
        EXP2 = ceil(EXP2 + (E1 / 16.0 + 17));
        EXP2 = clampEXP(EXP2);

        // Bound Sherlock's EXP within [0, 600] after Watson's correction
        EXP1 -= E1;
        EXP1 = clampEXP(EXP1);
    }
    if (E1 >= 66 && E1 <= 79) {
        // Information 1 for E1 in range [66, 79]
        EXP2 = ceil(EXP2 + (E1 / 4.0 + 19));
        EXP2 = clampEXP(EXP2);

        // Bound Sherlock's EXP within [0, 600] after Watson's correction
        EXP1 -= E1;
        EXP1 = clampEXP(EXP1);

        // Information 2 if EXP2 > 200
        if (EXP2 > 200) {
            EXP2 = ceil(EXP2 + (E1 / 9.0 + 21));
            EXP2 = clampEXP(EXP2);
        }
    }

    // Case 3: E1 is in [80, 99]
    if (E1 >= 80 && E1 <= 99) {
        // Information 1 for E1 in range [80, 99]
        EXP2 = ceil(EXP2 + (E1 / 4.0 + 19));
        EXP2 = clampEXP(EXP2);

        // Information 2 for E1 in range [80, 99]
        EXP2 = ceil(EXP2 + (E1 / 9.0 + 21));
        EXP2 = clampEXP(EXP2);

        // Bound Sherlock's EXP within [0, 600] after Watson's correction
        EXP1 -= E1;
        EXP1 = clampEXP(EXP1);

        // Information 3 if EXP2 > 400
        if (EXP2 > 400) {
            EXP2 = ceil(EXP2 + (E1 / 16.0 + 17));
            EXP2 = clampEXP(EXP2);

            // Apply the additional 15% for having all three pieces of information
            EXP2 = ceil(EXP2 * 1.15);
            EXP2 = clampEXP(EXP2);
        }
    }
    // Return the sum of EXP1 and EXP2.
    return EXP1 + EXP2;
}

// Task 2
void enforceConstraints(int& HP, int& EXP, int& M) {
    HP = max(0, min(HP, 666));
    EXP = max(0, min(EXP, 600));
    M = max(0, min(M, 3000));
}

// Function to determine the nearest perfect square to the given number
int nearestPerfectSquare(int num) {
    int root = static_cast<int>(sqrt(num));
    // Check which square is closer to num, root^2 or (root+1)^2
    return num - root*root <= (root+1)*(root+1) - num ? root*root : (root+1)*(root+1);
}

int clamp(int points, int minout, int maxout){
    return max(minout, min(points, maxout));
}

// Main function to describe Sherlock's luggage searching process

int traceLuggage(int& HP1, int& EXP1, int& M1, int E2) {
    if (E2 < 0 || E2 > 99) return -99;
    enforceConstraints(HP1, EXP1, M1); // Enforce constraints

    // Calculate the probability P1
    int S = nearestPerfectSquare(EXP1);
    double P1 = EXP1 >= S ? 1.0 : (static_cast<double>(EXP1) / (S) + 80)/123.0;

    // Calculate the probability P2 for Road 02
    // Initialize probability to 0
    double P2 = 0;
    // Road 02 logic
    bool isE2Odd = E2 % 2 != 0; // Check if E2 is odd
    int initialM = M1; // Starting money before beginning road 02
    int debugCounter = 0;
    //while ((initialM >= 150 && HP1 < 200) || (initialM >= 70 && HP1 >= 200)) { // If money is enough for grocery store or water
    while (M1>0) {

        // Event: Grocery store or water
        M1 = clamp(HP1 < 200 ? M1 - 150 : M1 - 70, 0, 3000);
        HP1 = clamp(ceil(HP1 < 200 ? HP1 * 130 / 100.0 : HP1 * 110 / 100.0), 0, 666);
        // Check if money spent exceeds 50% of initial money (if E2 is odd) or if money is not enough (if E2 is even)

        if (isE2Odd && (M1 < ceil(initialM / 2.0) || !M1)) { // If E2 is odd and money is exceeded 50% or money is not enough
            break; // Exit the loop
        } else if (!isE2Odd && !M1) { // If E2 is even and money is not enough
            break; // Exit the loop
        }

        // Event: Renting a taxi or carriage
        M1 = clamp(EXP1 < 400 ? M1 - 200 : M1 - 120, 0, 3000);
        EXP1 = clamp(ceil(EXP1 * 113 / 100.0), 0, 600);

        if (isE2Odd && (M1 < ceil(initialM / 2.0) || !M1)) { // If E2 is odd and money is exceeded 50% or money is not enough
            break; // Exit the loop
        } else if (!isE2Odd && !M1) { // If E2 is even and money is not enough
            break; // Exit the loop
        }

        // Event: Help a homeless person
        M1 = clamp(EXP1 < 300 ? M1 - 100 : M1 - 120, 0, 3000);
        EXP1 = clamp(ceil(EXP1 * 90 / 100.0), 0, 600);

        if (isE2Odd && (M1 < ceil(initialM / 2.0) || !M1)) { // If E2 is odd and money is exceeded 50% or money is not enough
            break; // Exit the loop
        } else if (!isE2Odd && !M1) { // If E2 is even and money is not enough
            break; // Exit the loop
        }

        debugCounter++;
        // If E2 is even, stop the loop
        if (!isE2Odd) { // If E2 is even and has done all events
            break;
        }
        // If E2 is odd
        if (isE2Odd && (M1 < ceil(initialM / 2.0) || !M1)) { // If E2 is odd and money is exceeded 50% or money is not enough
            break; // Exit the loop
        }
    }

    HP1 = clamp(ceil(HP1 * 83 / 100.0), 0, 666);
    EXP1 = clamp(ceil(EXP1 * 117 / 100.0), 0, 600);


    // Calculate the probability P2 for Road 02
    // Similar logic to P1 with the nearest perfect square to EXP1
    int S2 = nearestPerfectSquare(EXP1);
    P2 = EXP1 >= S2 ? 1.0 : (static_cast<double>(EXP1) / S2 + 80)/123.0;
    // Ensure P2 is within the range [0, 1]
    P2 = max(0.0, min(P2, 1.0));

    // Calculate the probability P3 for Road 03
    const int P[10] = {32, 47, 28, 79, 100, 50, 22, 83, 64, 11};
    // Determine index i based on E2
    int i = (E2 > 9) ? (E2 / 10 + E2 % 10) % 10 : E2;
    double P3 = P[i] / 100.0; // Convert to a probability

    // Calculate average probability of finding the suitcase
    double averageP = (P1 + P2 + P3) / 3;
    // Check for all 100% probabilities
    if(averageP == 1.0) {
        EXP1 = ceil(EXP1 * 0.75);

    } else {
        // Update HP1 and EXP1 based on the final probability
        HP1 = ceil(averageP < 0.5 ? HP1 * 85 / 100.0 : HP1 * 90 / 100.0);
        EXP1 = ceil(averageP < 0.5 ? EXP1 * 115 / 100.0 : EXP1 * 120 / 100.0);
    }

    // Ensure all values are within constraints
    enforceConstraints(HP1, EXP1, M1);

    // Return the sum of HP1, EXP1, and M1
    return HP1 + EXP1 + M1;
}
// Task 3
// Calculate the maximum value from both the left diagonal and right diagonal of the matrix
int maxValueDiagonal(int maxtrix[10][10],int row,int collumn) {
    int max = 0;
    // Start from current position and move to the upper left corner
    for (int i = row, j = collumn; i >= 0 && j >= 0 && i < 10 && j < 10; i--, j--) {
        if (maxtrix[i][j] > max) {
            max = maxtrix[i][j];
        }
    }
    // Start from current position and move to the upper right corner
    for (int i = row, j = collumn; i >= 0 && j >= 0 && i < 10 && j < 10; i--, j++) {
        if (maxtrix[i][j] > max) {
            max = maxtrix[i][j];
        }
    }
    // Start from current position and move to the lower left corner
    for (int i = row, j = collumn; i >= 0 && j >= 0 && i < 10 && j < 10; i++, j--) {
        if (maxtrix[i][j] > max) {
            max = maxtrix[i][j];
        }
    }
    // Start from current position and move to the lower right corner
    for (int i = row, j = collumn; i >= 0 && j >= 0 && i < 10 && j < 10; i++, j++) {
        if (maxtrix[i][j] > max) {
            max = maxtrix[i][j];
        }
    }
    return max;
}

int chaseTaxi(int &HP1, int &EXP1, int &HP2, int &EXP2, int E3)
{
    if (E3 < 0 || E3 > 99) return -99;
    HP1 = clamp(HP1, 0, 666);
    EXP1 = clamp(EXP1, 0, 600);
    HP2 = clamp(HP2, 0, 666);
    EXP2 = clamp(EXP2, 0, 600);
    // Initialize the coordinates of the taxi when Taxi would meet Sherlock and Watson at a point (x, y).
    int x = 0, y = 0;
    // Initialize the taxi matrix
    int taxiMatrix[10][10];
    // Calculate taxi's score at each point on the matrix
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            taxiMatrix[i][j] = ((E3 * j) + (i * 2)) * (i - j);
            if (taxiMatrix[i][j] > E3 * 2)
                x++;
            if (taxiMatrix[i][j] < -E3)
                y++;
        }
    }
    // Calculate the final point where the taxi would meet Sherlock and Watson
    while (x > 9){
        x = x % 10 + x / 10; // Calculate the sum of digits of x until x is less than 10
    }
    while (y > 9){
        y = y % 10 + y / 10; // Calculate the sum of digits of y until y is less than 10
    }
    // Calculate the final score of the taxi
    int taxiScore = taxiMatrix[x][y];
    // Calculate the final score of Sherlock and Watson
    int sherlockScore = maxValueDiagonal(taxiMatrix, x, y);
    // Update the HP and EXP of Sherlock and Watson based on the final scores
    EXP1 = clamp(ceil(abs(taxiScore) > sherlockScore ? EXP1 * 88 / 100.0 : EXP1 * 112 / 100.0), 0, 600);
    EXP2 = clamp(ceil(abs(taxiScore) > sherlockScore ? EXP2 * 88 / 100.0 : EXP2 * 112 / 100.0), 0, 600);
    HP1 = clamp(ceil(abs(taxiScore) > sherlockScore ? HP1 * 90 / 100.0 : HP1 * 110 / 100.0), 0, 666);
    HP2 = clamp(ceil(abs(taxiScore) > sherlockScore ? HP2 * 90 / 100.0 : HP2 * 110 / 100.0), 0, 666);
    return abs(taxiScore) > abs(sherlockScore) ? taxiScore : sherlockScore;
}


// Task 4
int checkPassword(const char *s, const char *email){
    // Get the username from the email
    string se = static_cast<std::string>(email).substr(0, static_cast<std::string>(email).find('@'));
    // Check if password is in range [8,20]
    if (strlen(s) < 8 || strlen(s) > 20) {
        return strlen(s) < 8 ? -1 : -2;
    }
    // Check if password contains the username and return position of the first occurrence of the username in the password
    if (strstr(s, se.c_str()) != NULL) {
        return -(300 + static_cast<int>(strstr(s, se.c_str()) - s));
    }
    // Check if contains more than 2 consecutive and identical characters, then return -(400+<sci>),
    for (int i = 0; i < strlen(s) - 2; i++) {
        if (s[i] == s[i + 1] && s[i] == s[i + 2]) {
            return -(400 + i);
        }
    }
    // Check if s does not contain special characters {'@', '#', '%', '$', '!'}, return -5
    if (strpbrk(s, "@#%$!") == NULL) {
        return -5;
    }
    // Check if s does not contain allowed special characters {'@', '#', '%', '$', '!'}, numbers, uppercase and lowercase letters, return current position of the first occurrence of the character that does not belong to the allowed set
    for (int i = 0; i < strlen(s); i++) {
        if (!isalnum(s[i]) && s[i] != '@' && s[i] != '#' && s[i] != '%' && s[i] != '$' && s[i] != '!') {
            return i;
        }
    }
    return -10;
}

// Task 5
int findCorrectPassword(const char * arr_pwds[], int num_pwds) {
    // Check what is the most password that has appeared in the array and return its first occurrence position
    string matrix[num_pwds][num_pwds];
    // Store password in the matrix [0][i] and the number of occurrences in the matrix [1][i]
    for (int i = 0; i < num_pwds; i++) {
        matrix[0][i] = arr_pwds[i];
        matrix[1][i] = "0";
    }
    // Count the number of occurrences of each password
    for (int i = 0; i < num_pwds; i++) {
        for (int j = 0; j < num_pwds; j++) {
            if (matrix[0][i] == arr_pwds[j]) {
                matrix[1][i] = to_string(stoi(matrix[1][i]) + 1);
            }
        }
    }
    // Find the most password that has appeared in the array
    int max = 0;
    int position = 0;
    for (int i = 0; i < num_pwds; i++) {
        if (stoi(matrix[1][i]) > max) {
            max = stoi(matrix[1][i]);
            position = i;
        }
        else if (stoi(matrix[1][i]) == max) { // If there are multiple passwords that have appeared the most, return the longest one
            if (strlen(matrix[0][i].c_str()) > strlen(matrix[0][position].c_str())) {
                position = i;
            }
        }
    }
    return position;

}


////////////////////////////////////////////////
/// END OF STUDENT'S ANSWER
////////////////////////////////////////////////