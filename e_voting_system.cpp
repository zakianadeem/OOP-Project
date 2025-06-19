#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include "raylib.h"

using namespace std;

// Screen dimensions
const int screenWidth = 1280;
const int screenHeight = 720;

// Colors
Color BACKGROUND_COLOR = {230, 230, 250, 255};
Color BUTTON_COLOR = {70, 130, 180, 255};
Color BUTTON_HOVER_COLOR = {100, 149, 237, 255};
Color TEXT_COLOR = BLACK;
Color INPUT_BOX_COLOR = LIGHTGRAY;
Color INPUT_BOX_ACTIVE_COLOR = WHITE;

// Font
Font mainFont;

// Input text buffers
char voterNameInput[256] = "\0";
char voterPassInput[256] = "\0";
char voterIdInput[256] = "\0";

char adminNameInput[256] = "\0";
char adminPassInput[256] = "\0";
char adminIdInput[256] = "\0";

char candidateNameInput[256] = "\0";
char candidatePartyInput[256] = "\0";
char candidateIdInput[256] = "\0";

char newVoterNameInput[256] = "\0";
char newVoterPassInput[256] = "\0";
char newVoterIdInput[256] = "\0";
char newVoterVotedInput[256] = "\0";

char removeCandidateIdInput[256] = "\0";
char removeVoterIdInput[256] = "\0";

char selectedCandidateInput[256] = "\0";

// Control variables
bool voterNameEditMode = false;
bool voterPassEditMode = false;
bool voterIdEditMode = false;

bool adminNameEditMode = false;
bool adminPassEditMode = false;
bool adminIdEditMode = false;

bool candidateNameEditMode = false;
bool candidatePartyEditMode = false;
bool candidateIdEditMode = false;

bool newVoterNameEditMode = false;
bool newVoterPassEditMode = false;
bool newVoterIdEditMode = false;
bool newVoterVotedEditMode = false;

bool removeCandidateIdEditMode = false;
bool removeVoterIdEditMode = false;

bool selectedCandidateEditMode = false;

// Application states
enum AppState {
    MAIN_MENU,
    VOTER_LOGIN,
    ADMIN_LOGIN,
    VOTER_PORTAL,
    ADMIN_PORTAL,
    VOTER_CHANGE_PASSWORD,
    VOTER_CAST_VOTE,
    VOTER_VIEW_INSTRUCTIONS,
    ADMIN_ADD_CANDIDATE,
    ADMIN_REMOVE_CANDIDATE,
    ADMIN_ADD_VOTER,
    ADMIN_REMOVE_VOTER,
    ADMIN_VIEW_RESULTS,
    ADMIN_CHECK_DUPLICATES
};

AppState currentState = MAIN_MENU;
AppState previousState = MAIN_MENU;

// Result message
string resultMessage = "";
float resultMessageTimer = 0.0f;
const float RESULT_MESSAGE_DURATION = 3.0f;

// Constants for array sizes
const int MAX_CANDIDATES = 100;
const int MAX_VOTERS = 1000;

// Helper functions
void ShowResultMessage(const string& message) {
    resultMessage = message;
    resultMessageTimer = RESULT_MESSAGE_DURATION;
}

void UpdateResultMessage(float deltaTime) {
    if (resultMessageTimer > 0) {
        resultMessageTimer -= deltaTime;
        if (resultMessageTimer <= 0) {
            resultMessage = "";
        }
    }
}

void DrawResultMessage() {
    if (!resultMessage.empty()) {
        int textWidth = MeasureText(resultMessage.c_str(), 20);
        DrawRectangle(screenWidth/2 - textWidth/2 - 20, 50, textWidth + 40, 40, Fade(BUTTON_COLOR, 0.9f));
        DrawText(resultMessage.c_str(), screenWidth/2 - textWidth/2, 60, 20, WHITE);
    }
}


void ResetInputFields() {
    memset(voterNameInput, 0, sizeof(voterNameInput));
    memset(voterPassInput, 0, sizeof(voterPassInput));
    memset(voterIdInput, 0, sizeof(voterIdInput));

    memset(adminNameInput, 0, sizeof(adminNameInput));
    memset(adminPassInput, 0, sizeof(adminPassInput));
    memset(adminIdInput, 0, sizeof(adminIdInput));

    memset(candidateNameInput, 0, sizeof(candidateNameInput));
    memset(candidatePartyInput, 0, sizeof(candidatePartyInput));
    memset(candidateIdInput, 0, sizeof(candidateIdInput));

    memset(newVoterNameInput, 0, sizeof(newVoterNameInput));
    memset(newVoterPassInput, 0, sizeof(newVoterPassInput));
    memset(newVoterIdInput, 0, sizeof(newVoterIdInput));
    memset(newVoterVotedInput, 0, sizeof(newVoterVotedInput));

    memset(removeCandidateIdInput, 0, sizeof(removeCandidateIdInput));
    memset(removeVoterIdInput, 0, sizeof(removeVoterIdInput));

    memset(selectedCandidateInput, 0, sizeof(selectedCandidateInput));
}

class User {
protected:
    string user_name;
    string password;
    int user_id;

public:
    User(string name, string pass, int id) : user_name(name), password(pass), user_id(id) {}

    int getID() {
        return user_id;
    }
    string getUser_name() {
        return user_name;
    }
    string getPassword() {
        return password;
    }

    bool login(string name, string pass, int id) {
        return (name == user_name && pass == password && id == user_id);
    }

    void change_password(string new_password) {
        password = new_password;
    }

    bool isAuthenticated(string name, string pass, int id) {
        return login(name, pass, id);
    }

    virtual void display() {
        cout << "Name: " << user_name << endl;
        cout << "ID: " << user_id << endl;
    }
};

class Candidate;

class Admin; // Forward declaration

class Voter : public User {
private:
    bool hasVoted;

public:
    Voter(string n, string p, int id, bool voted) : User(n, p, id), hasVoted(voted) {}

    bool getHasVoted() { return hasVoted; }
    void setHasVoted(bool voted) { hasVoted = voted; }

    void cast_vote(Candidate* c[], int count, Admin& admin, const string& candidateName);

    string viewInstructions() {
        return "To cast vote, press the Cast Vote button\nTo login, click the Login button";
    }
};

class Candidate {
private:
    string name;
    string party;
    int ID;
    int voteCount;

public:
    Candidate() : name(""), party(""), ID(0), voteCount(0) {}
    Candidate(string n, string p, int id) : name(n), party(p), ID(id), voteCount(0) {}

    int getID() {
        return ID;
    }

    string getName() const {
        return name;
    }

    string getParty() const {
        return party;
    }

    int getVoteCount() const {
        return voteCount;
    }

    friend void Voter::cast_vote(Candidate* c[], int count, Admin& admin, const string& candidateName);

    void incrementVote() {
        voteCount++;
    }

   string displayDetails() {
    string details = "• " + name + " (" + party + ")\n";
    details += "  ID: " + to_string(ID) + "\n";
    details += "  Votes: " + to_string(voteCount);
    return details;
}

    bool operator==(const Candidate& other) const {
    return (ID == other.ID && name == other.name);
}
};

class Admin : public User {
public:
    Candidate* candidates[MAX_CANDIDATES];
    Voter* voters[MAX_VOTERS];
    int candidateCount;
    int voterCount;

    Admin(string n, string p, int id) : User(n, p, id), candidateCount(0), voterCount(0) {
        for (int i = 0; i < MAX_CANDIDATES; i++) {
            candidates[i] = nullptr;
        }
        for (int i = 0; i < MAX_VOTERS; i++) {
            voters[i] = nullptr;
        }
    }

    ~Admin() {
        for (int i = 0; i < candidateCount; i++) {
            delete candidates[i];
        }
        for (int i = 0; i < voterCount; i++) {
            delete voters[i];
        }
    }

    void addCandidate(string n, string p, int id) {
        if (candidateCount < MAX_CANDIDATES) {
            candidates[candidateCount++] = new Candidate(n, p, id);
        }
    }
    void saveCandidatesToFile() {
    ofstream file("candidate_info.txt");
    if (!file) {
        ShowResultMessage("Error opening file to save candidates!");
        return;
    }

    for (int i = 0; i < candidateCount; i++) {
        file << "Name: " << candidates[i]->getName() << endl;
        file << "Party: " << candidates[i]->getParty() << endl;
        file << "ID: " << candidates[i]->getID() << endl;
        file << "Votes: " << candidates[i]->getVoteCount() << endl << endl;
    }

    file.close();
    ShowResultMessage("Candidates saved successfully.");
}

    void loadCandidatesFromFile() {
    ifstream file("candidate_info.txt");
    if (!file) {
        ShowResultMessage("Error opening file to load candidates!");
        return;
    }

    // Clear existing candidates
    for (int i = 0; i < candidateCount; i++) {
        delete candidates[i];
    }
    candidateCount = 0;

    string line;
    string name, party;
    int id = 0, votes = 0;

    while (getline(file, line)) {
        if (line.find("Name: ") == 0) {
            name = line.substr(6);

            // Read Party line
            if (!getline(file, line) || line.find("Party: ") != 0) {
                ShowResultMessage("Error: Missing Party information");
                continue;
            }
            party = line.substr(7);

            // Read ID line
            if (!getline(file, line) || line.find("ID: ") != 0) {
                ShowResultMessage("Error: Missing ID information");
                continue;
            }
            id = stoi(line.substr(4));

            // Read Votes line
            if (!getline(file, line) || line.find("Votes: ") != 0) {
                ShowResultMessage("Error: Missing Votes information");
                continue;
            }
            votes = stoi(line.substr(7));

            // Skip empty line if present
            while (file.peek() == '\n' || file.peek() == '\r') {
                file.ignore();
            }

            if (candidateCount < MAX_CANDIDATES) {
                candidates[candidateCount] = new Candidate(name, party, id);
                // Set votes directly
                for (int v = 0; v < votes; v++) {
                    candidates[candidateCount]->incrementVote();
                }
                candidateCount++;
            }
        }
    }

    file.close();
    ShowResultMessage("Candidates loaded successfully.");
}

    void updateCandidate(int id) {
    ifstream inFile("candidate_info.txt");
    if (!inFile) {
        ShowResultMessage("Error opening file for reading!");
        return;
    }

    ofstream tempFile("temp.txt");
    if (!tempFile) {
        ShowResultMessage("Error opening temporary file!");
        return;
    }

    string line, name, party;
    int candidateID, votes;
    bool found = false;

    while (getline(inFile, line)) {
        if (line.find("Name: ") == 0) {
            name = line.substr(6);
            getline(inFile, party);  // Read party line
            party = party.substr(7);

            if (getline(inFile, line) && line.find("ID: ") == 0) {
                candidateID = stoi(line.substr(4));
            }

            if (getline(inFile, line) && line.find("Votes: ") == 0) {
                votes = stoi(line.substr(7));
                if (candidateID == id) {
                    votes++;
                    found = true;
                }
            }

            // Write all fields back to temp file
            tempFile << "Name: " << name << endl;
            tempFile << "Party: " << party << endl;
            tempFile << "ID: " << candidateID << endl;
            tempFile << "Votes: " << votes << endl << endl;
        }
    }

    inFile.close();
    tempFile.close();

    remove("candidate_info.txt");
    rename("temp.txt", "candidate_info.txt");

    if (found) {
        ShowResultMessage("Candidate vote updated successfully in file.");
    } else {
        ShowResultMessage("Candidate not found in file.");
    }
}

    void removeCandidate(int id) {
        for (int i = 0; i < candidateCount; i++) {
            if (candidates[i]->getID() == id) {
                delete candidates[i];
                for (int j = i; j < candidateCount - 1; j++) {
                    candidates[j] = candidates[j + 1];
                }
                candidateCount--;
                candidates[candidateCount] = nullptr;
                ShowResultMessage("Candidate has been removed");
                return;
            }
        }
        ShowResultMessage("Candidate not found.");
    }

    void addVoter(string n, string p, int id, bool vote) {
        if (voterCount < MAX_VOTERS) {
            voters[voterCount++] = new Voter(n, p, id, vote);
        }
    }

    void removeVoter(int id) {
        for (int i = 0; i < voterCount; i++) {
            if (voters[i]->getID() == id) {
                delete voters[i];
                for (int j = i; j < voterCount - 1; j++) {
                    voters[j] = voters[j + 1];
                }
                voterCount--;
                voters[voterCount] = nullptr;
                ShowResultMessage("Voter has been removed");
                return;
            }
        }
        ShowResultMessage("Voter not found.");
    }

    string viewResult() {
    if (candidateCount == 0) {
        return "No candidates available.";
    }

    // Find the maximum vote count
    int maxVotes = 0;
    for (int i = 0; i < candidateCount; i++) {
        if (candidates[i]->getVoteCount() > maxVotes) {
            maxVotes = candidates[i]->getVoteCount();
        }
    }

    if (maxVotes == 0) {
        return "No votes have been cast yet.";
    }

    // Collect all candidates with max votes
    string result = "=== LEADING CANDIDATE(S) ===\n\n";
    bool first = true;
    for (int i = 0; i < candidateCount; i++) {
        if (candidates[i]->getVoteCount() == maxVotes) {
            if (!first) {
                result += "\n";  // Add separation between candidates
            }
            result += candidates[i]->getName() + " (" + candidates[i]->getParty() + ")\n";
            result += "ID: " + to_string(candidates[i]->getID()) + "\n";
            result += "Votes: " + to_string(candidates[i]->getVoteCount()) + "\n";
            first = false;
        }
    }

    // Add runner-ups if needed
    bool hasRunnerUps = false;
    string runnerUps = "\n=== RUNNER-UPS ===\n\n";
    for (int i = 0; i < candidateCount; i++) {
        if (candidates[i]->getVoteCount() > 0 &&
            candidates[i]->getVoteCount() < maxVotes) {
            runnerUps += candidates[i]->getName() + " (" + candidates[i]->getParty() + ")\n";
            runnerUps += "ID: " + to_string(candidates[i]->getID()) + "\n";
            runnerUps += "Votes: " + to_string(candidates[i]->getVoteCount()) + "\n\n";
            hasRunnerUps = true;
        }
    }

    if (hasRunnerUps) {
        result += runnerUps;
    }

    return result;
}

    string check_duplicate_candidates() {
        for (int i = 0; i < candidateCount; i++) {
            for (int j = i + 1; j < candidateCount; j++) {
                    if (*candidates[i] == *candidates[j]) {
                    return "Duplicate candidate found";
            }
            }
        }
        return "There is no duplicate candidate.";
    }

    string* getCandidateNames(int& count) {
        count = candidateCount;
        string* names = new string[count];
        for (int i = 0; i < count; i++) {
            names[i] = candidates[i]->getName();
        }
        return names;
    }

    string* getVoterNames(int& count) {
        count = voterCount;
        string* names = new string[count];
        for (int i = 0; i < count; i++) {
            names[i] = voters[i]->getUser_name();
        }
        return names;
    }
};

void Voter::cast_vote(Candidate* c[], int count, Admin& admin, const string& candidateName) {
    bool found = false;
    for (int i = 0; i < count; i++) {
        if (c[i]->getName() == candidateName) {
            if (hasVoted) {
                ShowResultMessage("You have already voted.");
            } else {
                c[i]->incrementVote();
                hasVoted = true;
                ShowResultMessage("Your vote has been casted.");
                admin.updateCandidate(c[i]->getID());
            }
            found = true;
            break;
        }
    }

    if (!found) {
        ShowResultMessage("No such candidate exists.");
    }
}

// Global objects
Voter* currentVoter = nullptr;
Admin admin("Admin", "admin", 1);

const int MAX_INSTRUCTIONS = 20;
string instructionsText[MAX_INSTRUCTIONS];
int instructionCount = 0;

const int MAX_CANDIDATE_DETAILS = 100;
string candidateDetails[MAX_CANDIDATE_DETAILS];
int candidateDetailCount = 0;

const int MAX_VOTER_DETAILS = 1000;
string voterDetails[MAX_VOTER_DETAILS];
int voterDetailCount = 0;

string resultsText;
string duplicatesText;

void InitializeSystem() {
    admin.addCandidate("Ali", "PTI", 1);
    admin.addVoter("Voter1", "vote1", 1, false);
    admin.addVoter("Voter2", "vote2", 2, false);

    // Load font
    mainFont = LoadFontEx("fonts/arial.ttf", 32, 0, 0);
    if (mainFont.texture.id == 0) {
        mainFont = GetFontDefault();
    }

    // Initialize instruction text
    string instructions = "Welcome to the E-Voting System\n\n"
                         "Instructions:\n"
                         "1. Voters can login and cast their vote\n"
                         "2. Admins can manage candidates and voters\n"
                         "3. Each voter can vote only once\n"
                         "4. Admins can view results after voting\n\n"
                         "Please select your role to begin.";

    size_t pos = 0;
    string delimiter = "\n";
    instructionCount = 0;
    string temp = instructions;

    while ((pos = temp.find(delimiter)) != string::npos && instructionCount < MAX_INSTRUCTIONS) {
        instructionsText[instructionCount++] = temp.substr(0, pos);
        temp.erase(0, pos + delimiter.length());
    }
    if (instructionCount < MAX_INSTRUCTIONS) {
        instructionsText[instructionCount++] = temp;
    }
}

void UpdateCandidateDetails(Admin& admin) {
    candidateDetailCount = 0;
    for (int i = 0; i < admin.candidateCount && candidateDetailCount < MAX_CANDIDATE_DETAILS; i++) {
        candidateDetails[candidateDetailCount++] =
            admin.candidates[i]->getName() + " (" + admin.candidates[i]->getParty() + ") - Votes: " +
            to_string(admin.candidates[i]->getVoteCount());
    }
}

void UpdateVoterDetails(Admin& admin) {
    voterDetailCount = 0;
    for (int i = 0; i < admin.voterCount && voterDetailCount < MAX_VOTER_DETAILS; i++) {
        voterDetails[voterDetailCount++] =
            admin.voters[i]->getUser_name() + " (ID: " + to_string(admin.voters[i]->getID()) +
            ") - Voted: " + (admin.voters[i]->getHasVoted() ? "Yes" : "No");
    }
}

void DrawMainMenu() {
    DrawText("E-Voting System", screenWidth/2 - MeasureText("E-Voting System", 40)/2, 100, 40, TEXT_COLOR);

    // Voter Login Button
    Rectangle voterBtn = {screenWidth/2 - 150, 200, 300, 50};
    bool voterBtnHover = CheckCollisionPointRec(GetMousePosition(), voterBtn);
    DrawRectangleRec(voterBtn, voterBtnHover ? BUTTON_HOVER_COLOR : BUTTON_COLOR);
    DrawText("Voter Login", voterBtn.x + voterBtn.width/2 - MeasureText("Voter Login", 20)/2,
             voterBtn.y + voterBtn.height/2 - 10, 20, WHITE);

    // Admin Login Button
    Rectangle adminBtn = {screenWidth/2 - 150, 280, 300, 50};
    bool adminBtnHover = CheckCollisionPointRec(GetMousePosition(), adminBtn);
    DrawRectangleRec(adminBtn, adminBtnHover ? BUTTON_HOVER_COLOR : BUTTON_COLOR);
    DrawText("Admin Login", adminBtn.x + adminBtn.width/2 - MeasureText("Admin Login", 20)/2,
             adminBtn.y + adminBtn.height/2 - 10, 20, WHITE);

    // Exit Button
    Rectangle exitBtn = {screenWidth/2 - 150, 360, 300, 50};
    bool exitBtnHover = CheckCollisionPointRec(GetMousePosition(), exitBtn);
    DrawRectangleRec(exitBtn, exitBtnHover ? RED : MAROON);
    DrawText("Exit", exitBtn.x + exitBtn.width/2 - MeasureText("Exit", 20)/2,
             exitBtn.y + exitBtn.height/2 - 10, 20, WHITE);

    // Handle button clicks
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (voterBtnHover) {
            currentState = VOTER_LOGIN;
            ResetInputFields();
        } else if (adminBtnHover) {
            currentState = ADMIN_LOGIN;
            ResetInputFields();
        } else if (exitBtnHover) {
            CloseWindow();
        }
    }
}

void DrawVoterLogin() {
    DrawText("Voter Login", screenWidth/2 - MeasureText("Voter Login", 30)/2, 100, 30, TEXT_COLOR);

    // Name Input
    Rectangle nameBox = {screenWidth/2 - 150, 180, 300, 40};
    DrawRectangleRec(nameBox, voterNameEditMode ? INPUT_BOX_ACTIVE_COLOR : INPUT_BOX_COLOR);
    DrawText("Name:", nameBox.x - 100, nameBox.y + 10, 20, TEXT_COLOR);
    DrawText(voterNameInput, nameBox.x + 10, nameBox.y + 10, 20, TEXT_COLOR);

    // Password Input
    Rectangle passBox = {screenWidth/2 - 150, 240, 300, 40};
    DrawRectangleRec(passBox, voterPassEditMode ? INPUT_BOX_ACTIVE_COLOR : INPUT_BOX_COLOR);
    DrawText("Password:", passBox.x - 100, passBox.y + 10, 20, TEXT_COLOR);
    DrawText(voterPassInput, passBox.x + 10, passBox.y + 10, 20, TEXT_COLOR);

    // ID Input
    Rectangle idBox = {screenWidth/2 - 150, 300, 300, 40};
    DrawRectangleRec(idBox, voterIdEditMode ? INPUT_BOX_ACTIVE_COLOR : INPUT_BOX_COLOR);
    DrawText("ID:", idBox.x - 100, idBox.y + 10, 20, TEXT_COLOR);
    DrawText(voterIdInput, idBox.x + 10, idBox.y + 10, 20, TEXT_COLOR);

    // Login Button
    Rectangle loginBtn = {screenWidth/2 - 100, 370, 200, 50};
    bool loginBtnHover = CheckCollisionPointRec(GetMousePosition(), loginBtn);
    DrawRectangleRec(loginBtn, loginBtnHover ? BUTTON_HOVER_COLOR : BUTTON_COLOR);
    DrawText("Login", loginBtn.x + loginBtn.width/2 - MeasureText("Login", 20)/2,
             loginBtn.y + loginBtn.height/2 - 10, 20, WHITE);

    // Back Button
    Rectangle backBtn = {screenWidth/2 - 100, 440, 200, 50};
    bool backBtnHover = CheckCollisionPointRec(GetMousePosition(), backBtn);
    DrawRectangleRec(backBtn, backBtnHover ? GRAY : DARKGRAY);
    DrawText("Back", backBtn.x + backBtn.width/2 - MeasureText("Back", 20)/2,
             backBtn.y + backBtn.height/2 - 10, 20, WHITE);

    // Handle input focus
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        voterNameEditMode = CheckCollisionPointRec(GetMousePosition(), nameBox);
        voterPassEditMode = CheckCollisionPointRec(GetMousePosition(), passBox);
        voterIdEditMode = CheckCollisionPointRec(GetMousePosition(), idBox);
    }

    // Handle text input
    if (voterNameEditMode) {
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= 32) && (key <= 125) && (strlen(voterNameInput) < 255)) {
                strncat(voterNameInput, (char*)&key, 1);
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && strlen(voterNameInput) > 0) {
            voterNameInput[strlen(voterNameInput) - 1] = '\0';
        }
    }

    if (voterPassEditMode) {
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= 32) && (key <= 125) && (strlen(voterPassInput) < 255)) {
                strncat(voterPassInput, (char*)&key, 1);
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && strlen(voterPassInput) > 0) {
            voterPassInput[strlen(voterPassInput) - 1] = '\0';
        }
    }

    if (voterIdEditMode) {
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= 48) && (key <= 57) && (strlen(voterIdInput) < 255)) {
                strncat(voterIdInput, (char*)&key, 1);
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && strlen(voterIdInput) > 0) {
            voterIdInput[strlen(voterIdInput) - 1] = '\0';
        }
    }

    // Handle button clicks
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (loginBtnHover) {
            string name = voterNameInput;
            string pass = voterPassInput;
            int id = voterIdInput[0] ? atoi(voterIdInput) : 0;

            bool authenticated = false;
            for (auto& voter : admin.voters) {
                if (voter->login(name, pass, id)) {
                    currentVoter = voter;
                    authenticated = true;
                    break;
                }
            }

            if (authenticated) {
                currentState = VOTER_PORTAL;
                ShowResultMessage("Voter authenticated successfully!");
            } else {
                ShowResultMessage("Authentication failed for voter!");
            }
        } else if (backBtnHover) {
            currentState = MAIN_MENU;
        }
    }
}

void DrawAdminLogin() {
    DrawText("Admin Login", screenWidth/2 - MeasureText("Admin Login", 30)/2, 100, 30, TEXT_COLOR);

    // Name Input
    Rectangle nameBox = {screenWidth/2 - 150, 180, 300, 40};
    DrawRectangleRec(nameBox, adminNameEditMode ? INPUT_BOX_ACTIVE_COLOR : INPUT_BOX_COLOR);
    DrawText("Name:", nameBox.x - 100, nameBox.y + 10, 20, TEXT_COLOR);
    DrawText(adminNameInput, nameBox.x + 10, nameBox.y + 10, 20, TEXT_COLOR);

    // Password Input
    Rectangle passBox = {screenWidth/2 - 150, 240, 300, 40};
    DrawRectangleRec(passBox, adminPassEditMode ? INPUT_BOX_ACTIVE_COLOR : INPUT_BOX_COLOR);
    DrawText("Password:", passBox.x - 100, passBox.y + 10, 20, TEXT_COLOR);
    DrawText(adminPassInput, passBox.x + 10, passBox.y + 10, 20, TEXT_COLOR);

    // ID Input
    Rectangle idBox = {screenWidth/2 - 150, 300, 300, 40};
    DrawRectangleRec(idBox, adminIdEditMode ? INPUT_BOX_ACTIVE_COLOR : INPUT_BOX_COLOR);
    DrawText("ID:", idBox.x - 100, idBox.y + 10, 20, TEXT_COLOR);
    DrawText(adminIdInput, idBox.x + 10, idBox.y + 10, 20, TEXT_COLOR);

    // Login Button
    Rectangle loginBtn = {screenWidth/2 - 100, 370, 200, 50};
    bool loginBtnHover = CheckCollisionPointRec(GetMousePosition(), loginBtn);
    DrawRectangleRec(loginBtn, loginBtnHover ? BUTTON_HOVER_COLOR : BUTTON_COLOR);
    DrawText("Login", loginBtn.x + loginBtn.width/2 - MeasureText("Login", 20)/2,
             loginBtn.y + loginBtn.height/2 - 10, 20, WHITE);

    // Back Button
    Rectangle backBtn = {screenWidth/2 - 100, 440, 200, 50};
    bool backBtnHover = CheckCollisionPointRec(GetMousePosition(), backBtn);
    DrawRectangleRec(backBtn, backBtnHover ? GRAY : DARKGRAY);
    DrawText("Back", backBtn.x + backBtn.width/2 - MeasureText("Back", 20)/2,
             backBtn.y + backBtn.height/2 - 10, 20, WHITE);

    // Handle input focus
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        adminNameEditMode = CheckCollisionPointRec(GetMousePosition(), nameBox);
        adminPassEditMode = CheckCollisionPointRec(GetMousePosition(), passBox);
        adminIdEditMode = CheckCollisionPointRec(GetMousePosition(), idBox);
    }

    // Handle text input
    if (adminNameEditMode) {
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= 32) && (key <= 125) && (strlen(adminNameInput) < 255)) {
                strncat(adminNameInput, (char*)&key, 1);
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && strlen(adminNameInput) > 0) {
            adminNameInput[strlen(adminNameInput) - 1] = '\0';
        }
    }

    if (adminPassEditMode) {
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= 32) && (key <= 125) && (strlen(adminPassInput) < 255)) {
                strncat(adminPassInput, (char*)&key, 1);
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && strlen(adminPassInput) > 0) {
            adminPassInput[strlen(adminPassInput) - 1] = '\0';
        }
    }

    if (adminIdEditMode) {
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= 48) && (key <= 57) && (strlen(adminIdInput) < 255)) {
                strncat(adminIdInput, (char*)&key, 1);
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && strlen(adminIdInput) > 0) {
            adminIdInput[strlen(adminIdInput) - 1] = '\0';
        }
    }

    // Handle button clicks
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (loginBtnHover) {
            string name = adminNameInput;
            string pass = adminPassInput;
            int id = adminIdInput[0] ? atoi(adminIdInput) : 0;

            if (admin.isAuthenticated(name, pass, id)) {
                currentState = ADMIN_PORTAL;
                admin.loadCandidatesFromFile();
                ShowResultMessage("Admin authenticated successfully!");
            } else {
                ShowResultMessage("Admin authentication failed!");
            }
        } else if (backBtnHover) {
            currentState = MAIN_MENU;
        }
    }
}

void DrawVoterPortal() {
    DrawText("Voter Portal", screenWidth/2 - MeasureText("Voter Portal", 30)/2, 50, 30, TEXT_COLOR);

    // Welcome message
    string welcomeMsg = "Welcome, " + currentVoter->getUser_name() + "!";
    DrawText(welcomeMsg.c_str(), screenWidth/2 - MeasureText(welcomeMsg.c_str(), 24)/2, 100, 24, TEXT_COLOR);

    // Change Password Button
    Rectangle changePassBtn = {screenWidth/2 - 150, 160, 300, 50};
    bool changePassBtnHover = CheckCollisionPointRec(GetMousePosition(), changePassBtn);
    DrawRectangleRec(changePassBtn, changePassBtnHover ? BUTTON_HOVER_COLOR : BUTTON_COLOR);
    DrawText("Change Password", changePassBtn.x + changePassBtn.width/2 - MeasureText("Change Password", 20)/2,
             changePassBtn.y + changePassBtn.height/2 - 10, 20, WHITE);

    // Cast Vote Button
    Rectangle castVoteBtn = {screenWidth/2 - 150, 230, 300, 50};
    bool castVoteBtnHover = CheckCollisionPointRec(GetMousePosition(), castVoteBtn);
    DrawRectangleRec(castVoteBtn, castVoteBtnHover ? BUTTON_HOVER_COLOR : BUTTON_COLOR);
    DrawText("Cast Vote", castVoteBtn.x + castVoteBtn.width/2 - MeasureText("Cast Vote", 20)/2,
             castVoteBtn.y + castVoteBtn.height/2 - 10, 20, WHITE);

    // View Instructions Button
    Rectangle viewInstrBtn = {screenWidth/2 - 150, 300, 300, 50};
    bool viewInstrBtnHover = CheckCollisionPointRec(GetMousePosition(), viewInstrBtn);
    DrawRectangleRec(viewInstrBtn, viewInstrBtnHover ? BUTTON_HOVER_COLOR : BUTTON_COLOR);
    DrawText("View Instructions", viewInstrBtn.x + viewInstrBtn.width/2 - MeasureText("View Instructions", 20)/2,
             viewInstrBtn.y + viewInstrBtn.height/2 - 10, 20, WHITE);

    // Logout Button
    Rectangle logoutBtn = {screenWidth/2 - 150, 370, 300, 50};
    bool logoutBtnHover = CheckCollisionPointRec(GetMousePosition(), logoutBtn);
    DrawRectangleRec(logoutBtn, logoutBtnHover ? GRAY : DARKGRAY);
    DrawText("Logout", logoutBtn.x + logoutBtn.width/2 - MeasureText("Logout", 20)/2,
             logoutBtn.y + logoutBtn.height/2 - 10, 20, WHITE);

    // Handle button clicks
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (changePassBtnHover) {
            previousState = currentState;
            currentState = VOTER_CHANGE_PASSWORD;
        } else if (castVoteBtnHover) {
            previousState = currentState;
            currentState = VOTER_CAST_VOTE;
        } else if (viewInstrBtnHover) {
            previousState = currentState;
            currentState = VOTER_VIEW_INSTRUCTIONS;
        } else if (logoutBtnHover) {
            currentState = MAIN_MENU;
            currentVoter = nullptr;
        }
    }
}

void DrawVoterChangePassword() {
    DrawText("Change Password", screenWidth/2 - MeasureText("Change Password", 30)/2, 100, 30, TEXT_COLOR);

    // New Password Input
    Rectangle newPassBox = {screenWidth/2 - 150, 180, 300, 40};
    DrawRectangleRec(newPassBox, voterPassEditMode ? INPUT_BOX_ACTIVE_COLOR : INPUT_BOX_COLOR);
    DrawText("New Password:", newPassBox.x - 150, newPassBox.y + 10, 20, TEXT_COLOR);
    DrawText(voterPassInput, newPassBox.x + 10, newPassBox.y + 10, 20, TEXT_COLOR);

    // Confirm Button
    Rectangle confirmBtn = {screenWidth/2 - 100, 250, 200, 50};
    bool confirmBtnHover = CheckCollisionPointRec(GetMousePosition(), confirmBtn);
    DrawRectangleRec(confirmBtn, confirmBtnHover ? BUTTON_HOVER_COLOR : BUTTON_COLOR);
    DrawText("Confirm", confirmBtn.x + confirmBtn.width/2 - MeasureText("Confirm", 20)/2,
             confirmBtn.y + confirmBtn.height/2 - 10, 20, WHITE);

    // Back Button
    Rectangle backBtn = {screenWidth/2 - 100, 320, 200, 50};
    bool backBtnHover = CheckCollisionPointRec(GetMousePosition(), backBtn);
    DrawRectangleRec(backBtn, backBtnHover ? GRAY : DARKGRAY);
    DrawText("Back", backBtn.x + backBtn.width/2 - MeasureText("Back", 20)/2,
             backBtn.y + backBtn.height/2 - 10, 20, WHITE);

    // Handle input focus
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        voterPassEditMode = CheckCollisionPointRec(GetMousePosition(), newPassBox);
    }

    // Handle text input
    if (voterPassEditMode) {
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= 32) && (key <= 125) && (strlen(voterPassInput) < 255)) {
                strncat(voterPassInput, (char*)&key, 1);
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && strlen(voterPassInput) > 0) {
            voterPassInput[strlen(voterPassInput) - 1] = '\0';
        }
    }

    // Handle button clicks
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (confirmBtnHover) {
            string newPass = voterPassInput;
            currentVoter->change_password(newPass);
            ShowResultMessage("Password changed successfully!");
            memset(voterPassInput, 0, sizeof(voterPassInput));
        } else if (backBtnHover) {
            currentState = previousState;
            memset(voterPassInput, 0, sizeof(voterPassInput));
        }
    }
}

void DrawVoterViewInstructions() {
    DrawText("Instructions", screenWidth/2 - MeasureText("Instructions", 30)/2, 50, 30, TEXT_COLOR);

    // Display instructions
    int yPos = 120;
    for (int i = 0; i < instructionCount; i++) {
        DrawText(instructionsText[i].c_str(),
                screenWidth/2 - MeasureText(instructionsText[i].c_str(), 20)/2,
                yPos, 20, TEXT_COLOR);
        yPos += 30;
    }

    // Back button
    Rectangle backBtn = {screenWidth/2 - 100, yPos + 30, 200, 50};
    bool backBtnHover = CheckCollisionPointRec(GetMousePosition(), backBtn);
    DrawRectangleRec(backBtn, backBtnHover ? GRAY : DARKGRAY);
    DrawText("Back", backBtn.x + backBtn.width/2 - MeasureText("Back", 20)/2,
             backBtn.y + backBtn.height/2 - 10, 20, WHITE);

    // Handle button clicks
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && backBtnHover) {
        currentState = previousState;
    }
}

void DrawVoterCastVote() {
    DrawText("Cast Your Vote", screenWidth/2 - MeasureText("Cast Your Vote", 30)/2, 50, 30, TEXT_COLOR);

    // Display candidates with their party names
    int yPos = 120;
    for (int i = 0; i < admin.candidateCount; i++) {
        string candidateText = admin.candidates[i]->getName() + " (" + admin.candidates[i]->getParty() + ")";
        DrawText(candidateText.c_str(),
                screenWidth/2 - MeasureText(candidateText.c_str(), 20)/2,
                yPos, 20, TEXT_COLOR);
        yPos += 30;
    }
    // Selected Candidate Input
    Rectangle candidateBox = {screenWidth/2 - 150, yPos + 20, 300, 40};
    DrawRectangleRec(candidateBox, selectedCandidateEditMode ? INPUT_BOX_ACTIVE_COLOR : INPUT_BOX_COLOR);
    DrawText("Select Candidate:", candidateBox.x - 150, candidateBox.y + 10, 20, TEXT_COLOR);
    DrawText(selectedCandidateInput, candidateBox.x + 10, candidateBox.y + 10, 20, TEXT_COLOR);

    // Vote Button
    Rectangle voteBtn = {screenWidth/2 - 100, yPos + 80, 200, 50};
    bool voteBtnHover = CheckCollisionPointRec(GetMousePosition(), voteBtn);
    DrawRectangleRec(voteBtn, voteBtnHover ? BUTTON_HOVER_COLOR : BUTTON_COLOR);
    DrawText("Vote", voteBtn.x + voteBtn.width/2 - MeasureText("Vote", 20)/2,
             voteBtn.y + voteBtn.height/2 - 10, 20, WHITE);

    // Back Button
    Rectangle backBtn = {screenWidth/2 - 100, yPos + 150, 200, 50};
    bool backBtnHover = CheckCollisionPointRec(GetMousePosition(), backBtn);
    DrawRectangleRec(backBtn, backBtnHover ? GRAY : DARKGRAY);
    DrawText("Back", backBtn.x + backBtn.width/2 - MeasureText("Back", 20)/2,
             backBtn.y + backBtn.height/2 - 10, 20, WHITE);

    // Handle input focus
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        selectedCandidateEditMode = CheckCollisionPointRec(GetMousePosition(), candidateBox);
    }

    // Handle text input
    if (selectedCandidateEditMode) {
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= 32) && (key <= 125) && (strlen(selectedCandidateInput) < 255)) {
                strncat(selectedCandidateInput, (char*)&key, 1);
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && strlen(selectedCandidateInput) > 0) {
            selectedCandidateInput[strlen(selectedCandidateInput) - 1] = '\0';
        }
    }

    // Handle button clicks
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (voteBtnHover) {
            string candidateName = selectedCandidateInput;
          currentVoter->cast_vote(admin.candidates, admin.candidateCount, admin, candidateName);
            memset(selectedCandidateInput, 0, sizeof(selectedCandidateInput));
        } else if (backBtnHover) {
            currentState = previousState;
            memset(selectedCandidateInput, 0, sizeof(selectedCandidateInput));
        }
    }
}

void DrawAdminPortal() {
    DrawText("Admin Portal", screenWidth/2 - MeasureText("Admin Portal", 30)/2, 50, 30, TEXT_COLOR);

    // Add Candidate Button
    Rectangle addCandBtn = {screenWidth/2 - 300, 120, 250, 50};
    bool addCandBtnHover = CheckCollisionPointRec(GetMousePosition(), addCandBtn);
    DrawRectangleRec(addCandBtn, addCandBtnHover ? BUTTON_HOVER_COLOR : BUTTON_COLOR);
    DrawText("Add Candidate", addCandBtn.x + addCandBtn.width/2 - MeasureText("Add Candidate", 20)/2,
             addCandBtn.y + addCandBtn.height/2 - 10, 20, WHITE);

    // Remove Candidate Button
    Rectangle remCandBtn = {screenWidth/2 + 50, 120, 250, 50};
    bool remCandBtnHover = CheckCollisionPointRec(GetMousePosition(), remCandBtn);
    DrawRectangleRec(remCandBtn, remCandBtnHover ? BUTTON_HOVER_COLOR : BUTTON_COLOR);
    DrawText("Remove Candidate", remCandBtn.x + remCandBtn.width/2 - MeasureText("Remove Candidate", 20)/2,
             remCandBtn.y + remCandBtn.height/2 - 10, 20, WHITE);

    // Add Voter Button
    Rectangle addVoterBtn = {screenWidth/2 - 300, 190, 250, 50};
    bool addVoterBtnHover = CheckCollisionPointRec(GetMousePosition(), addVoterBtn);
    DrawRectangleRec(addVoterBtn, addVoterBtnHover ? BUTTON_HOVER_COLOR : BUTTON_COLOR);
    DrawText("Add Voter", addVoterBtn.x + addVoterBtn.width/2 - MeasureText("Add Voter", 20)/2,
             addVoterBtn.y + addVoterBtn.height/2 - 10, 20, WHITE);

    // Remove Voter Button
    Rectangle remVoterBtn = {screenWidth/2 + 50, 190, 250, 50};
    bool remVoterBtnHover = CheckCollisionPointRec(GetMousePosition(), remVoterBtn);
    DrawRectangleRec(remVoterBtn, remVoterBtnHover ? BUTTON_HOVER_COLOR : BUTTON_COLOR);
    DrawText("Remove Voter", remVoterBtn.x + remVoterBtn.width/2 - MeasureText("Remove Voter", 20)/2,
             remVoterBtn.y + remVoterBtn.height/2 - 10, 20, WHITE);

    // View Results Button
    Rectangle viewResBtn = {screenWidth/2 - 300, 260, 250, 50};
    bool viewResBtnHover = CheckCollisionPointRec(GetMousePosition(), viewResBtn);
    DrawRectangleRec(viewResBtn, viewResBtnHover ? BUTTON_HOVER_COLOR : BUTTON_COLOR);
    DrawText("View Results", viewResBtn.x + viewResBtn.width/2 - MeasureText("View Results", 20)/2,
             viewResBtn.y + viewResBtn.height/2 - 10, 20, WHITE);

    // Check Duplicates Button
    Rectangle checkDupBtn = {screenWidth/2 + 50, 260, 250, 50};
    bool checkDupBtnHover = CheckCollisionPointRec(GetMousePosition(), checkDupBtn);
    DrawRectangleRec(checkDupBtn, checkDupBtnHover ? BUTTON_HOVER_COLOR : BUTTON_COLOR);
    DrawText("Check Duplicates", checkDupBtn.x + checkDupBtn.width/2 - MeasureText("Check Duplicates", 20)/2,
             checkDupBtn.y + checkDupBtn.height/2 - 10, 20, WHITE);

    // Save Data Button
    Rectangle saveBtn = {screenWidth/2 - 300, 330, 250, 50};
    bool saveBtnHover = CheckCollisionPointRec(GetMousePosition(), saveBtn);
    DrawRectangleRec(saveBtn, saveBtnHover ? GREEN : DARKGREEN);
    DrawText("Save Data", saveBtn.x + saveBtn.width/2 - MeasureText("Save Data", 20)/2,
             saveBtn.y + saveBtn.height/2 - 10, 20, WHITE);

    // Load Data Button
    Rectangle loadBtn = {screenWidth/2 + 50, 330, 250, 50};
    bool loadBtnHover = CheckCollisionPointRec(GetMousePosition(), loadBtn);
    DrawRectangleRec(loadBtn, loadBtnHover ? GREEN : DARKGREEN);
    DrawText("Load Data", loadBtn.x + loadBtn.width/2 - MeasureText("Load Data", 20)/2,
             loadBtn.y + loadBtn.height/2 - 10, 20, WHITE);

    // Logout Button
    Rectangle logoutBtn = {screenWidth/2 - 100, 410, 200, 50};
    bool logoutBtnHover = CheckCollisionPointRec(GetMousePosition(), logoutBtn);
    DrawRectangleRec(logoutBtn, logoutBtnHover ? GRAY : DARKGRAY);
    DrawText("Logout", logoutBtn.x + logoutBtn.width/2 - MeasureText("Logout", 20)/2,
             logoutBtn.y + logoutBtn.height/2 - 10, 20, WHITE);

    // Handle button clicks
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (addCandBtnHover) {
            previousState = currentState;
            currentState = ADMIN_ADD_CANDIDATE;
            ResetInputFields();
        } else if (remCandBtnHover) {
            previousState = currentState;
            currentState = ADMIN_REMOVE_CANDIDATE;
            ResetInputFields();
        } else if (addVoterBtnHover) {
            previousState = currentState;
            currentState = ADMIN_ADD_VOTER;
            ResetInputFields();
        } else if (remVoterBtnHover) {
            previousState = currentState;
            currentState = ADMIN_REMOVE_VOTER;
            ResetInputFields();
        } else if (viewResBtnHover) {
            previousState = currentState;
            currentState = ADMIN_VIEW_RESULTS;
            resultsText = admin.viewResult();
        } else if (checkDupBtnHover) {
            previousState = currentState;
            currentState = ADMIN_CHECK_DUPLICATES;
            duplicatesText = admin.check_duplicate_candidates();
        } else if (saveBtnHover) {
            admin.saveCandidatesToFile();
        } else if (loadBtnHover) {
            admin.loadCandidatesFromFile();
        } else if (logoutBtnHover) {
            currentState = MAIN_MENU;
        }
    }
}

void DrawAdminAddCandidate() {
    DrawText("Add Candidate", screenWidth/2 - MeasureText("Add Candidate", 30)/2, 50, 30, TEXT_COLOR);

    // Name Input
    Rectangle nameBox = {screenWidth/2 - 150, 120, 300, 40};
    DrawRectangleRec(nameBox, candidateNameEditMode ? INPUT_BOX_ACTIVE_COLOR : INPUT_BOX_COLOR);
    DrawText("Name:", nameBox.x - 100, nameBox.y + 10, 20, TEXT_COLOR);
    DrawText(candidateNameInput, nameBox.x + 10, nameBox.y + 10, 20, TEXT_COLOR);

    // Party Input
    Rectangle partyBox = {screenWidth/2 - 150, 180, 300, 40};
    DrawRectangleRec(partyBox, candidatePartyEditMode ? INPUT_BOX_ACTIVE_COLOR : INPUT_BOX_COLOR);
    DrawText("Party:", partyBox.x - 100, partyBox.y + 10, 20, TEXT_COLOR);
    DrawText(candidatePartyInput, partyBox.x + 10, partyBox.y + 10, 20, TEXT_COLOR);

    // ID Input
    Rectangle idBox = {screenWidth/2 - 150, 240, 300, 40};
    DrawRectangleRec(idBox, candidateIdEditMode ? INPUT_BOX_ACTIVE_COLOR : INPUT_BOX_COLOR);
    DrawText("ID:", idBox.x - 100, idBox.y + 10, 20, TEXT_COLOR);
    DrawText(candidateIdInput, idBox.x + 10, idBox.y + 10, 20, TEXT_COLOR);

    // Add Button
    Rectangle addBtn = {screenWidth/2 - 100, 310, 200, 50};
    bool addBtnHover = CheckCollisionPointRec(GetMousePosition(), addBtn);
    DrawRectangleRec(addBtn, addBtnHover ? BUTTON_HOVER_COLOR : BUTTON_COLOR);
    DrawText("Add", addBtn.x + addBtn.width/2 - MeasureText("Add", 20)/2,
             addBtn.y + addBtn.height/2 - 10, 20, WHITE);

    // Back Button
    Rectangle backBtn = {screenWidth/2 - 100, 380, 200, 50};
    bool backBtnHover = CheckCollisionPointRec(GetMousePosition(), backBtn);
    DrawRectangleRec(backBtn, backBtnHover ? GRAY : DARKGRAY);
    DrawText("Back", backBtn.x + backBtn.width/2 - MeasureText("Back", 20)/2,
             backBtn.y + backBtn.height/2 - 10, 20, WHITE);

    // Handle input focus
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        candidateNameEditMode = CheckCollisionPointRec(GetMousePosition(), nameBox);
        candidatePartyEditMode = CheckCollisionPointRec(GetMousePosition(), partyBox);
        candidateIdEditMode = CheckCollisionPointRec(GetMousePosition(), idBox);
    }

    // Handle text input
    if (candidateNameEditMode) {
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= 32) && (key <= 125) && (strlen(candidateNameInput) < 255)) {
                strncat(candidateNameInput, (char*)&key, 1);
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && strlen(candidateNameInput) > 0) {
            candidateNameInput[strlen(candidateNameInput) - 1] = '\0';
        }
    }

    if (candidatePartyEditMode) {
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= 32) && (key <= 125) && (strlen(candidatePartyInput) < 255)) {
                strncat(candidatePartyInput, (char*)&key, 1);
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && strlen(candidatePartyInput) > 0) {
            candidatePartyInput[strlen(candidatePartyInput) - 1] = '\0';
        }
    }

    if (candidateIdEditMode) {
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= 48) && (key <= 57) && (strlen(candidateIdInput) < 255)) {
                strncat(candidateIdInput, (char*)&key, 1);
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && strlen(candidateIdInput) > 0) {
            candidateIdInput[strlen(candidateIdInput) - 1] = '\0';
        }
    }

    // Handle button clicks
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (addBtnHover) {
            string name = candidateNameInput;
            string party = candidatePartyInput;
            int id = candidateIdInput[0] ? atoi(candidateIdInput) : 0;

            if (!name.empty() && !party.empty() && id != 0) {
                admin.addCandidate(name, party, id);
                ShowResultMessage("Candidate added successfully!");
                memset(candidateNameInput, 0, sizeof(candidateNameInput));
                memset(candidatePartyInput, 0, sizeof(candidatePartyInput));
                memset(candidateIdInput, 0, sizeof(candidateIdInput));
            } else {
                ShowResultMessage("Please fill all fields correctly!");
            }
        } else if (backBtnHover) {
            currentState = previousState;
            memset(candidateNameInput, 0, sizeof(candidateNameInput));
            memset(candidatePartyInput, 0, sizeof(candidatePartyInput));
            memset(candidateIdInput, 0, sizeof(candidateIdInput));
        }
    }
}

void DrawAdminRemoveCandidate() {
    DrawText("Remove Candidate", screenWidth/2 - MeasureText("Remove Candidate", 30)/2, 50, 30, TEXT_COLOR);

    // Display current candidates
    int yPos = 100;
    for (int i = 0; i < admin.candidateCount ; i++) {
        string candidateText = to_string(admin.candidates[i]->getID()) + " - " + admin.candidates[i]->getName();
        DrawText(candidateText.c_str(), screenWidth/2 - MeasureText(candidateText.c_str(), 20)/2, yPos, 20, TEXT_COLOR);
        yPos += 30;
    }

    // ID Input
    Rectangle idBox = {screenWidth/2 - 150, yPos + 20, 300, 40};
    DrawRectangleRec(idBox, removeCandidateIdEditMode ? INPUT_BOX_ACTIVE_COLOR : INPUT_BOX_COLOR);
    DrawText("Candidate ID:", idBox.x - 150, idBox.y + 10, 20, TEXT_COLOR);
    DrawText(removeCandidateIdInput, idBox.x + 10, idBox.y + 10, 20, TEXT_COLOR);

    // Remove Button
    Rectangle removeBtn = {screenWidth/2 - 100, yPos + 80, 200, 50};
    bool removeBtnHover = CheckCollisionPointRec(GetMousePosition(), removeBtn);
    DrawRectangleRec(removeBtn, removeBtnHover ? BUTTON_HOVER_COLOR : BUTTON_COLOR);
    DrawText("Remove", removeBtn.x + removeBtn.width/2 - MeasureText("Remove", 20)/2,
             removeBtn.y + removeBtn.height/2 - 10, 20, WHITE);

    // Back Button
    Rectangle backBtn = {screenWidth/2 - 100, yPos + 150, 200, 50};
    bool backBtnHover = CheckCollisionPointRec(GetMousePosition(), backBtn);
    DrawRectangleRec(backBtn, backBtnHover ? GRAY : DARKGRAY);
    DrawText("Back", backBtn.x + backBtn.width/2 - MeasureText("Back", 20)/2,
             backBtn.y + backBtn.height/2 - 10, 20, WHITE);

    // Handle input focus
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        removeCandidateIdEditMode = CheckCollisionPointRec(GetMousePosition(), idBox);
    }

    // Handle text input
    if (removeCandidateIdEditMode) {
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= 48) && (key <= 57) && (strlen(removeCandidateIdInput) < 255)) {
                strncat(removeCandidateIdInput, (char*)&key, 1);
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && strlen(removeCandidateIdInput) > 0) {
            removeCandidateIdInput[strlen(removeCandidateIdInput) - 1] = '\0';
        }
    }

    // Handle button clicks
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (removeBtnHover) {
            int id = removeCandidateIdInput[0] ? atoi(removeCandidateIdInput) : 0;
            if (id != 0) {
                admin.removeCandidate(id);
                memset(removeCandidateIdInput, 0, sizeof(removeCandidateIdInput));
            } else {
                ShowResultMessage("Please enter a valid ID!");
            }
        } else if (backBtnHover) {
            currentState = previousState;
            memset(removeCandidateIdInput, 0, sizeof(removeCandidateIdInput));
        }
    }
}

void DrawAdminAddVoter() {
    DrawText("Add Voter", screenWidth/2 - MeasureText("Add Voter", 30)/2, 50, 30, TEXT_COLOR);

    // Name Input
    Rectangle nameBox = {screenWidth/2 - 150, 120, 300, 40};
    DrawRectangleRec(nameBox, newVoterNameEditMode ? INPUT_BOX_ACTIVE_COLOR : INPUT_BOX_COLOR);
    DrawText("Name:", nameBox.x - 100, nameBox.y + 10, 20, TEXT_COLOR);
    DrawText(newVoterNameInput, nameBox.x + 10, nameBox.y + 10, 20, TEXT_COLOR);

    // Password Input
    Rectangle passBox = {screenWidth/2 - 150, 180, 300, 40};
    DrawRectangleRec(passBox, newVoterPassEditMode ? INPUT_BOX_ACTIVE_COLOR : INPUT_BOX_COLOR);
    DrawText("Password:", passBox.x - 100, passBox.y + 10, 20, TEXT_COLOR);
    DrawText(newVoterPassInput, passBox.x + 10, passBox.y + 10, 20, TEXT_COLOR);

    // ID Input
    Rectangle idBox = {screenWidth/2 - 150, 240, 300, 40};
    DrawRectangleRec(idBox, newVoterIdEditMode ? INPUT_BOX_ACTIVE_COLOR : INPUT_BOX_COLOR);
    DrawText("ID:", idBox.x - 100, idBox.y + 10, 20, TEXT_COLOR);
    DrawText(newVoterIdInput, idBox.x + 10, idBox.y + 10, 20, TEXT_COLOR);

    // Voted Status Input
    Rectangle votedBox = {screenWidth/2 - 150, 300, 300, 40};
    DrawRectangleRec(votedBox, newVoterVotedEditMode ? INPUT_BOX_ACTIVE_COLOR : INPUT_BOX_COLOR);
    DrawText("Voted (0/1):", votedBox.x - 120, votedBox.y + 10, 20, TEXT_COLOR);
    DrawText(newVoterVotedInput, votedBox.x + 10, votedBox.y + 10, 20, TEXT_COLOR);

    // Add Button
    Rectangle addBtn = {screenWidth/2 - 100, 370, 200, 50};
    bool addBtnHover = CheckCollisionPointRec(GetMousePosition(), addBtn);
    DrawRectangleRec(addBtn, addBtnHover ? BUTTON_HOVER_COLOR : BUTTON_COLOR);
    DrawText("Add", addBtn.x + addBtn.width/2 - MeasureText("Add", 20)/2,
             addBtn.y + addBtn.height/2 - 10, 20, WHITE);

    // Back Button
    Rectangle backBtn = {screenWidth/2 - 100, 440, 200, 50};
    bool backBtnHover = CheckCollisionPointRec(GetMousePosition(), backBtn);
    DrawRectangleRec(backBtn, backBtnHover ? GRAY : DARKGRAY);
    DrawText("Back", backBtn.x + backBtn.width/2 - MeasureText("Back", 20)/2,
             backBtn.y + backBtn.height/2 - 10, 20, WHITE);

    // Handle input focus
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        newVoterNameEditMode = CheckCollisionPointRec(GetMousePosition(), nameBox);
        newVoterPassEditMode = CheckCollisionPointRec(GetMousePosition(), passBox);
        newVoterIdEditMode = CheckCollisionPointRec(GetMousePosition(), idBox);
        newVoterVotedEditMode = CheckCollisionPointRec(GetMousePosition(), votedBox);
    }

    // Handle text input
    if (newVoterNameEditMode) {
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= 32) && (key <= 125) && (strlen(newVoterNameInput) < 255)) {
                strncat(newVoterNameInput, (char*)&key, 1);
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && strlen(newVoterNameInput) > 0) {
            newVoterNameInput[strlen(newVoterNameInput) - 1] = '\0';
        }
    }

    if (newVoterPassEditMode) {
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= 32) && (key <= 125) && (strlen(newVoterPassInput) < 255)) {
                strncat(newVoterPassInput, (char*)&key, 1);
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && strlen(newVoterPassInput) > 0) {
            newVoterPassInput[strlen(newVoterPassInput) - 1] = '\0';
        }
    }

    if (newVoterIdEditMode) {
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= 48) && (key <= 57) && (strlen(newVoterIdInput) < 255)) {
                strncat(newVoterIdInput, (char*)&key, 1);
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && strlen(newVoterIdInput) > 0) {
            newVoterIdInput[strlen(newVoterIdInput) - 1] = '\0';
        }
    }

    if (newVoterVotedEditMode) {
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= 48) && (key <= 49) && (strlen(newVoterVotedInput) < 255)) {
                strncat(newVoterVotedInput, (char*)&key, 1);
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && strlen(newVoterVotedInput) > 0) {
            newVoterVotedInput[strlen(newVoterVotedInput) - 1] = '\0';
        }
    }

    // Handle button clicks
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (addBtnHover) {
            string name = newVoterNameInput;
            string pass = newVoterPassInput;
            int id = newVoterIdInput[0] ? atoi(newVoterIdInput) : 0;
            bool voted = newVoterVotedInput[0] ? (newVoterVotedInput[0] == '1') : false;

            if (!name.empty() && !pass.empty() && id != 0) {
                admin.addVoter(name, pass, id, voted);
                ShowResultMessage("Voter added successfully!");
                memset(newVoterNameInput, 0, sizeof(newVoterNameInput));
                memset(newVoterPassInput, 0, sizeof(newVoterPassInput));
                memset(newVoterIdInput, 0, sizeof(newVoterIdInput));
                memset(newVoterVotedInput, 0, sizeof(newVoterVotedInput));
            } else {
                ShowResultMessage("Please fill all fields correctly!");
            }
        } else if (backBtnHover) {
            currentState = previousState;
            memset(newVoterNameInput, 0, sizeof(newVoterNameInput));
            memset(newVoterPassInput, 0, sizeof(newVoterPassInput));
            memset(newVoterIdInput, 0, sizeof(newVoterIdInput));
            memset(newVoterVotedInput, 0, sizeof(newVoterVotedInput));
        }
    }
}

void DrawAdminRemoveVoter() {
    DrawText("Remove Voter", screenWidth/2 - MeasureText("Remove Voter", 30)/2, 50, 30, TEXT_COLOR);

    // Display current voters
    int yPos = 100;
    for (int i = 0; i < admin.voterCount; i++) {
        string voterText = to_string(admin.voters[i]->getID()) + " - " + admin.voters[i]->getUser_name();
        DrawText(voterText.c_str(), screenWidth/2 - MeasureText(voterText.c_str(), 20)/2, yPos, 20, TEXT_COLOR);
        yPos += 30;
    }

    // ID Input
    Rectangle idBox = {screenWidth/2 - 150, yPos + 20, 300, 40};
    DrawRectangleRec(idBox, removeVoterIdEditMode ? INPUT_BOX_ACTIVE_COLOR : INPUT_BOX_COLOR);
    DrawText("Voter ID:", idBox.x - 120, idBox.y + 10, 20, TEXT_COLOR);
    DrawText(removeVoterIdInput, idBox.x + 10, idBox.y + 10, 20, TEXT_COLOR);

    // Remove Button
    Rectangle removeBtn = {screenWidth/2 - 100, yPos + 80, 200, 50};
    bool removeBtnHover = CheckCollisionPointRec(GetMousePosition(), removeBtn);
    DrawRectangleRec(removeBtn, removeBtnHover ? BUTTON_HOVER_COLOR : BUTTON_COLOR);
    DrawText("Remove", removeBtn.x + removeBtn.width/2 - MeasureText("Remove", 20)/2,
             removeBtn.y + removeBtn.height/2 - 10, 20, WHITE);

    // Back Button
    Rectangle backBtn = {screenWidth/2 - 100, yPos + 150, 200, 50};
    bool backBtnHover = CheckCollisionPointRec(GetMousePosition(), backBtn);
    DrawRectangleRec(backBtn, backBtnHover ? GRAY : DARKGRAY);
    DrawText("Back", backBtn.x + backBtn.width/2 - MeasureText("Back", 20)/2,
             backBtn.y + backBtn.height/2 - 10, 20, WHITE);

    // Handle input focus
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        removeVoterIdEditMode = CheckCollisionPointRec(GetMousePosition(), idBox);
    }

    // Handle text input
    if (removeVoterIdEditMode) {
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= 48) && (key <= 57) && (strlen(removeVoterIdInput) < 255)) {
                strncat(removeVoterIdInput, (char*)&key, 1);
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && strlen(removeVoterIdInput) > 0) {
            removeVoterIdInput[strlen(removeVoterIdInput) - 1] = '\0';
        }
    }

    // Handle button clicks
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (removeBtnHover) {
            int id = removeVoterIdInput[0] ? atoi(removeVoterIdInput) : 0;
            if (id != 0) {
                admin.removeVoter(id);
                memset(removeVoterIdInput, 0, sizeof(removeVoterIdInput));
            } else {
                ShowResultMessage("Please enter a valid ID!");
            }
        } else if (backBtnHover) {
            currentState = previousState;
            memset(removeVoterIdInput, 0, sizeof(removeVoterIdInput));
        }
    }
}

void DrawAdminViewResults() {
    DrawText("Voting Results", screenWidth/2 - MeasureText("Voting Results", 30)/2, 50, 30, TEXT_COLOR);

    // Display results
    int yPos = 100;
    const int MAX_RESULT_LINES = 20;
    string resultLines[MAX_RESULT_LINES];
    int lineCount = 0;

    size_t pos = 0;
    string delimiter = "\n";
    string tempResults = resultsText;

    // Split the results text into lines
    while ((pos = tempResults.find(delimiter)) != string::npos && lineCount < MAX_RESULT_LINES) {
        resultLines[lineCount++] = tempResults.substr(0, pos);
        tempResults.erase(0, pos + delimiter.length());
    }

    // Add the last line if there's space
    if (lineCount < MAX_RESULT_LINES && !tempResults.empty()) {
        resultLines[lineCount++] = tempResults;
    }

    // Draw each line of results
    for (int i = 0; i < lineCount; i++) {
        DrawText(resultLines[i].c_str(), screenWidth/2 - MeasureText(resultLines[i].c_str(), 20)/2, yPos, 20, TEXT_COLOR);
        yPos += 30;
    }

    // Back Button
    Rectangle backBtn = {screenWidth/2 - 100, yPos + 50, 200, 50};
    bool backBtnHover = CheckCollisionPointRec(GetMousePosition(), backBtn);
    DrawRectangleRec(backBtn, backBtnHover ? GRAY : DARKGRAY);
    DrawText("Back", backBtn.x + backBtn.width/2 - MeasureText("Back", 20)/2,
             backBtn.y + backBtn.height/2 - 10, 20, WHITE);

    // Handle button clicks
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (backBtnHover) {
            currentState = previousState;
        }
    }
}

void DrawAdminCheckDuplicates() {
    DrawText("Check Duplicate Candidates", screenWidth/2 - MeasureText("Check Duplicate Candidates", 30)/2, 50, 30, TEXT_COLOR);

    // Display duplicate check result
    DrawText(duplicatesText.c_str(), screenWidth/2 - MeasureText(duplicatesText.c_str(), 20)/2, 120, 20, TEXT_COLOR);

    // Back Button
    Rectangle backBtn = {screenWidth/2 - 100, 200, 200, 50};
    bool backBtnHover = CheckCollisionPointRec(GetMousePosition(), backBtn);
    DrawRectangleRec(backBtn, backBtnHover ? GRAY : DARKGRAY);
    DrawText("Back", backBtn.x + backBtn.width/2 - MeasureText("Back", 20)/2,
             backBtn.y + backBtn.height/2 - 10, 20, WHITE);

    // Handle button clicks
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (backBtnHover) {
            currentState = previousState;
        }
    }
}
// main function
int main() {
    // Initialize window
    InitWindow(screenWidth, screenHeight, "E-Voting System");
    SetTargetFPS(60);

    InitializeSystem();

    // Main game loop
    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        UpdateResultMessage(deltaTime);

        BeginDrawing();
        ClearBackground(BACKGROUND_COLOR);

        // Draw based on current state
        switch (currentState) {
            case MAIN_MENU:
                DrawMainMenu();
                break;
            case VOTER_LOGIN:
                DrawVoterLogin();
                break;
            case ADMIN_LOGIN:
                DrawAdminLogin();
                break;
            case VOTER_PORTAL:
                DrawVoterPortal();
                break;
            case VOTER_CHANGE_PASSWORD:
                DrawVoterChangePassword();
                break;
            case VOTER_CAST_VOTE:
                DrawVoterCastVote();
                break;
            case VOTER_VIEW_INSTRUCTIONS:
                DrawVoterViewInstructions();
                break;
            case ADMIN_PORTAL:
                DrawAdminPortal();
                break;
            case ADMIN_ADD_CANDIDATE:
                DrawAdminAddCandidate();
                break;
            case ADMIN_REMOVE_CANDIDATE:
                DrawAdminRemoveCandidate();
                break;
            case ADMIN_ADD_VOTER:
                DrawAdminAddVoter();
                break;
            case ADMIN_REMOVE_VOTER:
                DrawAdminRemoveVoter();
                break;
            case ADMIN_VIEW_RESULTS:
                DrawAdminViewResults();
                break;
            case ADMIN_CHECK_DUPLICATES:
                DrawAdminCheckDuplicates();
                break;
        }

        // Draw result message
        DrawResultMessage();

        EndDrawing();
    }

    // Close window
    CloseWindow();

    return 0;
}
