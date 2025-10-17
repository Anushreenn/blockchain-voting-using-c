#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>  // SHA-256 Encryption

#define MAX_CANDIDATES 3
#define MAX_VOTERS 100

// Structure to store a blockchain block (vote)
typedef struct Block {
    int voterID;
    char candidate[50];
    unsigned char prevHash[SHA256_DIGEST_LENGTH];
    unsigned char currHash[SHA256_DIGEST_LENGTH];
    struct Block* next;
} Block;

Block* blockchain = NULL;

// Function to compute SHA-256 hash
void computeHash(const char* data, unsigned char hash[SHA256_DIGEST_LENGTH]) {
    SHA256((unsigned char*)data, strlen(data), hash);
}

// Function to print a hash
void printHash(unsigned char hash[SHA256_DIGEST_LENGTH]) {
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        printf("%02x", hash[i]);
    }
}

// Function to add a new vote (block) to the blockchain
void addVote(int voterID, char candidate[]) {
    Block* newBlock = (Block*)malloc(sizeof(Block));
    newBlock->voterID = voterID;
    strcpy(newBlock->candidate, candidate);
    newBlock->next = NULL;

    // Compute current hash (hash of voterID + candidate)
    char data[100];
    sprintf(data, "%d%s", voterID, candidate);
    computeHash(data, newBlock->currHash);

    // Link to previous block
    if (blockchain == NULL) {
        memset(newBlock->prevHash, 0, SHA256_DIGEST_LENGTH); // First block (Genesis Block)
    } else {
        memcpy(newBlock->prevHash, blockchain->currHash, SHA256_DIGEST_LENGTH);
    }

    // Add to blockchain
    newBlock->next = blockchain;
    blockchain = newBlock;

    // Save vote to file
    FILE* file = fopen("votes.txt", "a");
    if (file) {
        fprintf(file, "Voter ID: %d, Candidate: %s, Hash: ", voterID, candidate);
        for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
            fprintf(file, "%02x", newBlock->currHash[i]);
        }
        fprintf(file, "\n");
        fclose(file);
    }

    printf("✅ Vote successfully recorded!\n");
}

// Function to display all votes (Blockchain)
void displayVotes() {
    Block* current = blockchain;
    printf("\n===== Blockchain Votes =====\n");
    while (current) {
        printf("Voter ID: %d, Candidate: %s\n", current->voterID, current->candidate);
        printf("Previous Hash: "); printHash(current->prevHash); printf("\n");
        printf("Current Hash: "); printHash(current->currHash); printf("\n");
        printf("------------------------------------\n");
        current = current->next;
    }
}

// Function to validate blockchain integrity
void validateBlockchain() {
    Block* current = blockchain;
    while (current && current->next) {
        if (memcmp(current->next->prevHash, current->currHash, SHA256_DIGEST_LENGTH) != 0) {
            printf("\n🔴 Blockchain Integrity Compromised!\n");
            return;
        }
        current = current->next;
    }
    printf("\n✅ Blockchain is Valid and Secure!\n");
}

// Admin login function
int adminLogin() {
    char username[20], password[20];
    printf("\nEnter Admin Username: ");
    scanf("%s", username);
    printf("Enter Admin Password: ");
    scanf("%s", password);

    if (strcmp(username, "admin") == 0 && strcmp(password, "1234") == 0) {
        printf("\n✅ Admin Login Successful!\n");
        return 1;
    } else {
        printf("\n🔴 Invalid Admin Credentials!\n");
        return 0;
    }
}

// Main function
int main() {
    int choice, voterID;
    char candidate[50];
    
    while (1) {
        printf("\n===== Blockchain Voting System =====\n");
        printf("1. Vote\n");
        printf("2. Display Votes (Admin)\n");
        printf("3. Validate Blockchain (Admin)\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("\nEnter Voter ID: ");
                scanf("%d", &voterID);
                printf("Enter Candidate Name: ");
                scanf("%s", candidate);
                addVote(voterID, candidate);
                break;

            case 2:
                if (adminLogin()) displayVotes();
                break;

            case 3:
                if (adminLogin()) validateBlockchain();
                break;

            case 4:
                printf("Exiting...\n");
                return 0;

            default:
                printf("Invalid choice! Try again.\n");
        }
    }
    return 0;
}
