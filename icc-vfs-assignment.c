#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Players_data.h"

#define MAX_TEAM 10
#define MAX_NAME_LEN 64

typedef enum { ROLE_BATSMAN = 1, ROLE_BOWLER = 2, ROLE_ALLROUNDER = 3 } Role;

typedef struct MyPlayer {
    int id;
    char name[MAX_NAME_LEN];
    char teamName[MAX_NAME_LEN];
    Role role;
    int totalRuns;
    double battingAverage;
    double strikeRate;
    int wickets;
    double economyRate;
    double perfIndex;
    struct MyPlayer *next;
} MyPlayer;

typedef struct Team {
    int TeamId;
    char Name[MAX_NAME_LEN];
    int TotalPlayers;
    double sumBattingStrike;
    int countBatters;
    double AvgBattingStrikeRate;
    MyPlayer *batsmen_head;
    MyPlayer *bowlers_head;
    MyPlayer *allround_head;
} Team;

Team teams_global[MAX_TEAM];
int team_count = 0;

static void trimnl(char *s) {
    if (!s) return;
    size_t L = strlen(s);
    while (L && (s[L-1] == '\n' || s[L-1] == '\r')) s[--L] = '\0';
}

static Role str_to_role(const char *s) {
    if (!s) return ROLE_BATSMAN;
    if (strcasecmp(s, "Batsman") == 0) return ROLE_BATSMAN;
    if (strcasecmp(s, "Bowler") == 0) return ROLE_BOWLER;
    return ROLE_ALLROUNDER;
}

static double compute_perf_index(const MyPlayer *p) {
    if (!p) return 0.0;
    if (p->role == ROLE_BATSMAN) return (p->battingAverage * p->strikeRate) / 100.0;
    else if (p->role == ROLE_BOWLER) return (p->wickets * 2.0) + (100.0 - p->economyRate);
    else return ((p->battingAverage * p->strikeRate) / 100.0) + (p->wickets * 2.0);
}

static MyPlayer* create_player_node(int id, const char *name, const char *teamName,
        Role role, int runs, double avg, double sr, int wkts, double er) {
    MyPlayer *p = malloc(sizeof(MyPlayer));
    if (!p) exit(1);
    p->id = id;
    strncpy(p->name, name, MAX_NAME_LEN-1); p->name[MAX_NAME_LEN-1] = '\0';
    strncpy(p->teamName, teamName, MAX_NAME_LEN-1); p->teamName[MAX_NAME_LEN-1] = '\0';
    p->role = role;
    p->totalRuns = runs;
    p->battingAverage = avg;
    p->strikeRate = sr;
    p->wickets = wkts;
    p->economyRate = er;
    p->perfIndex = compute_perf_index(p);
    p->next = NULL;
    return p;
}

static void insert_sorted_desc(MyPlayer **headRef, MyPlayer *node) {
    if (!headRef || !node) return;
    MyPlayer *head = *headRef;
    if (!head || node->perfIndex > head->perfIndex) {
        node->next = head;
        *headRef = node;
        return;
    }
    MyPlayer *cur = head;
    while (cur->next && cur->next->perfIndex >= node->perfIndex) cur = cur->next;
    node->next = cur->next;
    cur->next = node;
}

static void update_team_avg(Team *t, MyPlayer *p, int adding) {
    if (!t || !p) return;
    int is_batter = (p->role == ROLE_BATSMAN || p->role == ROLE_ALLROUNDER);
    if (adding) {
        t->TotalPlayers++;
        if (is_batter) {
            t->sumBattingStrike += p->strikeRate;
            t->countBatters++;
        }
    } else {
        t->TotalPlayers--;
        if (is_batter) {
            t->sumBattingStrike -= p->strikeRate;
            t->countBatters--;
        }
    }
    t->AvgBattingStrikeRate = t->countBatters ? t->sumBattingStrike / t->countBatters : 0.0;
}

static int team_binary_search(Team arr[], int n, int TeamId) {
    int lo = 0, hi = n-1;
    while (lo <= hi) {
        int mid = lo + (hi - lo)/2;
        if (arr[mid].TeamId == TeamId) return mid;
        if (arr[mid].TeamId < TeamId) lo = mid + 1;
        else hi = mid - 1;
    }
    return -1;
}

static Team* get_team_by_id(int TeamId) {
    int idx = team_binary_search(teams_global, team_count, TeamId);
    return (idx == -1) ? NULL : &teams_global[idx];
}

static void init_teams_from_header() {
    team_count = teamCount;
    for (int i = 0; i < team_count; ++i) {
        teams_global[i].TeamId = i + 1;
        strncpy(teams_global[i].Name, teams[i], MAX_NAME_LEN-1);
        teams_global[i].Name[MAX_NAME_LEN-1] = '\0';
        teams_global[i].TotalPlayers = 0;
        teams_global[i].sumBattingStrike = 0.0;
        teams_global[i].countBatters = 0;
        teams_global[i].AvgBattingStrikeRate = 0.0;
        teams_global[i].batsmen_head = NULL;
        teams_global[i].bowlers_head = NULL;
        teams_global[i].allround_head = NULL;
    }
}

static void add_player_to_team(MyPlayer *p) {
    if (!p) return;
    int idx = -1;
    for (int i = 0; i < team_count; ++i) {
        if (strcmp(teams_global[i].Name, p->teamName) == 0) { idx = i; break; }
    }
    if (idx == -1) {
        free(p);
        return;
    }
    Team *t = &teams_global[idx];
    if (p->role == ROLE_BATSMAN) insert_sorted_desc(&t->batsmen_head, p);
    else if (p->role == ROLE_BOWLER) insert_sorted_desc(&t->bowlers_head, p);
    else insert_sorted_desc(&t->allround_head, p);
    update_team_avg(t, p, 1);
}

static void load_initial_players() {
    for (int i = 0; i < playerCount; ++i) {
        const Player *hp = &players[i];
        Role r = str_to_role(hp->role);
        MyPlayer *p = create_player_node(hp->id, hp->name, hp->team, r,
                                         hp->totalRuns, hp->battingAverage, hp->strikeRate,
                                         hp->wickets, hp->economyRate);
        add_player_to_team(p);
    }
}

static void display_players_of_team(Team *t) {
    if (!t) return;
    printf("\nPlayers of Team %s:\n", t->Name);
    printf("====================================================================================\n");
    printf("ID    Name                           Role         Runs     Avg     SR     Wkts   ER    PerfIdx\n");
    printf("------------------------------------------------------------------------------------\n");

    MyPlayer *cur = t->batsmen_head;
    while (cur) {
        printf("%-5d %-30s %-11s %-8d %-7.1f %-6.1f %-6d %-5.1f %-8.2f\n",
            cur->id, cur->name, "Batsman", cur->totalRuns, cur->battingAverage,
            cur->strikeRate, cur->wickets, cur->economyRate, cur->perfIndex);
        cur = cur->next;
    }
    cur = t->allround_head;
    while (cur) {
        printf("%-5d %-30s %-11s %-8d %-7.1f %-6.1f %-6d %-5.1f %-8.2f\n",
            cur->id, cur->name, "All-rounder", cur->totalRuns, cur->battingAverage,
            cur->strikeRate, cur->wickets, cur->economyRate, cur->perfIndex);
        cur = cur->next;
    }
    cur = t->bowlers_head;
    while (cur) {
        printf("%-5d %-30s %-11s %-8d %-7.1f %-6.1f %-6d %-5.1f %-8.2f\n",
            cur->id, cur->name, "Bowler", cur->totalRuns, cur->battingAverage,
            cur->strikeRate, cur->wickets, cur->economyRate, cur->perfIndex);
        cur = cur->next;
    }
    printf("====================================================================================\n");
    printf("Total Players: %d\n", t->TotalPlayers);
    printf("Average Batting Strike Rate: %.2f\n\n", t->AvgBattingStrikeRate);
}

static int cmp_team_avg_desc(const void *a, const void *b) {
    const Team *ta = &teams_global[*(int*)a];
    const Team *tb = &teams_global[*(int*)b];
    if (ta->AvgBattingStrikeRate < tb->AvgBattingStrikeRate) return 1;
    if (ta->AvgBattingStrikeRate > tb->AvgBattingStrikeRate) return -1;
    return 0;
}

static void display_teams_by_avg_strike() {
    int idxs[MAX_TEAM];
    for (int i = 0; i < team_count; ++i) idxs[i] = i;
    qsort(idxs, team_count, sizeof(int), cmp_team_avg_desc);
    printf("\nTeams Sorted by Average Batting Strike Rate (Desc):\n");
    printf("============================================================\n");
    printf("ID   Team Name                 Avg Bat SR   Total Players\n");
    printf("------------------------------------------------------------\n");
    for (int i = 0; i < team_count; ++i) {
        Team *t = &teams_global[idxs[i]];
        printf("%-4d %-25s %-12.2f %-5d\n", t->TeamId, t->Name, t->AvgBattingStrikeRate, t->TotalPlayers);
    }
    printf("============================================================\n\n");
}

static void display_top_k_team_role(Team *t, Role role, int K) {
    if (!t) return;
    MyPlayer *head = NULL;
    if (role == ROLE_BATSMAN) head = t->batsmen_head;
    else if (role == ROLE_BOWLER) head = t->bowlers_head;
    else head = t->allround_head;

    if (!head) {
        printf("No players of selected role in team %s\n", t->Name);
        return;
    }

    printf("\nTop %d players of Team %s:\n", K, t->Name);
    printf("====================================================================================\n");
    printf("ID    Name                           Role         Runs     Avg     SR     Wkts   ER    PerfIdx\n");
    printf("------------------------------------------------------------------------------------\n");

    MyPlayer *cur = head;
    int count = 0;
    while (cur && count < K) {
        printf("%-5d %-30s %-11s %-8d %-7.1f %-6.1f %-6d %-5.1f %-8.2f\n",
            cur->id, cur->name,
            role==ROLE_BATSMAN?"Batsman": role==ROLE_BOWLER?"Bowler":"All-rounder",
            cur->totalRuns, cur->battingAverage, cur->strikeRate,
            cur->wickets, cur->economyRate, cur->perfIndex);
        cur = cur->next;
        ++count;
    }
    printf("====================================================================================\n\n");
}

typedef struct HeapNode {
    MyPlayer *p;
    int team_idx;
} HeapNode;

typedef struct MaxHeap {
    HeapNode *arr;
    int size;
    int cap;
} MaxHeap;

static MaxHeap* heap_create(int cap) {
    MaxHeap *h = malloc(sizeof(MaxHeap));
    h->arr = malloc(sizeof(HeapNode)*cap);
    h->size = 0; h->cap = cap;
    return h;
}

static void heap_free(MaxHeap *h) {
    if (!h) return;
    free(h->arr);
    free(h);
}

static void heap_swap(HeapNode *a, HeapNode *b) {
    HeapNode tmp = *a; *a = *b; *b = tmp;
}

static void heap_push(MaxHeap *h, HeapNode node) {
    if (h->size >= h->cap) return;
    int i = h->size++;
    h->arr[i] = node;
    while (i > 0) {
        int parent = (i-1)/2;
        if (h->arr[parent].p->perfIndex >= h->arr[i].p->perfIndex) break;
        heap_swap(&h->arr[parent], &h->arr[i]);
        i = parent;
    }
}

static HeapNode heap_pop(MaxHeap *h) {
    HeapNode res = h->arr[0];
    h->arr[0] = h->arr[--h->size];
    int i = 0;
    while (1) {
        int l = 2*i + 1, r = 2*i + 2, largest = i;
        if (l < h->size && h->arr[l].p->perfIndex > h->arr[largest].p->perfIndex) largest = l;
        if (r < h->size && h->arr[r].p->perfIndex > h->arr[largest].p->perfIndex) largest = r;
        if (largest == i) break;
        heap_swap(&h->arr[i], &h->arr[largest]);
        i = largest;
    }
    return res;
}

static void display_all_players_role_across_teams(Role role) {
    MaxHeap *h = heap_create(team_count);
    for (int i = 0; i < team_count; ++i) {
        MyPlayer *node = NULL;
        if (role == ROLE_BATSMAN) node = teams_global[i].batsmen_head;
        else if (role == ROLE_BOWLER) node = teams_global[i].bowlers_head;
        else node = teams_global[i].allround_head;
        if (node) {
            HeapNode hn = { node, i };
            heap_push(h, hn);
        }
    }

    printf("\nAll players across all teams:\n");
    printf("====================================================================================\n");
    printf("ID    Name                           Team                Role         Runs   Avg   SR   Wkts  ER    PerfIdx\n");
    printf("------------------------------------------------------------------------------------\n");

    while (h->size > 0) {
        HeapNode hn = heap_pop(h);
        MyPlayer *p = hn.p;
        Team *t = &teams_global[hn.team_idx];
        printf("%-5d %-30s %-18s %-11s %-6d %-6.1f %-5.1f %-5d %-5.1f %-8.2f\n",
               p->id, p->name, t->Name,
               role==ROLE_BATSMAN?"Batsman": role==ROLE_BOWLER?"Bowler":"All-rounder",
               p->totalRuns, p->battingAverage, p->strikeRate, p->wickets, p->economyRate, p->perfIndex);

        MyPlayer *nextp = p->next;
        if (nextp) {
            HeapNode newhn = { nextp, hn.team_idx };
            heap_push(h, newhn);
        }
    }
    printf("====================================================================================\n\n");
    heap_free(h);
}

static int read_int() {
    char buf[256];
    if (!fgets(buf, sizeof(buf), stdin)) return -1;
    trimnl(buf);
    return atoi(buf);
}

static void read_str(const char *prompt, char *out, int maxlen) {
    printf("%s", prompt);
    if (!fgets(out, maxlen, stdin)) { out[0]='\0'; return; }
    trimnl(out);
}

static void interactive_add_player() {
    printf("Enter Team ID (1-%d): ", team_count);
    int tid = read_int();
    Team *t = get_team_by_id(tid);
    if (!t) return;
    char name[128]; char temp[32];
    int pid;
    char teamname[MAX_NAME_LEN];
    strcpy(teamname, t->Name);

    printf("Player ID: ");
    pid = read_int();
    read_str("Name: ", name, sizeof(name));
    printf("Role (1-Batsman, 2-Bowler, 3-All-rounder): ");
    int r = read_int();
    read_str("Total Runs: ", temp, sizeof(temp)); int runs = atoi(temp);
    read_str("Batting Average: ", temp, sizeof(temp)); double avg = atof(temp);
    read_str("Strike Rate: ", temp, sizeof(temp)); double sr = atof(temp);
    read_str("Wickets: ", temp, sizeof(temp)); int wkts = atoi(temp);
    read_str("Economy Rate: ", temp, sizeof(temp)); double er = atof(temp);

    MyPlayer *p = create_player_node(pid, name, teamname, (Role)r, runs, avg, sr, wkts, er);

    if (p->role == ROLE_BATSMAN) insert_sorted_desc(&t->batsmen_head, p);
    else if (p->role == ROLE_BOWLER) insert_sorted_desc(&t->bowlers_head, p);
    else insert_sorted_desc(&t->allround_head, p);

    update_team_avg(t, p, 1);
    printf("Player added successfully\n");
}

static void free_all_memory() {
    for (int i = 0; i < team_count; ++i) {
        Team *t = &teams_global[i];
        MyPlayer *cur, *nxt;
        cur = t->batsmen_head;
        while (cur) { nxt = cur->next; free(cur); cur = nxt; }
        cur = t->allround_head;
        while (cur) { nxt = cur->next; free(cur); cur = nxt; }
        cur = t->bowlers_head;
        while (cur) { nxt = cur->next; free(cur); cur = nxt; }
    }
}

static void print_menu() {
    printf("=====================================================================\n");
    printf("ICC ODI Player Performance Analyzer\n");
    printf("=====================================================================\n");
    printf("1. Add Player to Team\n");
    printf("2. Display Players of a Specific Team\n");
    printf("3. Display Teams by Average Batting Strike Rate\n");
    printf("4. Display Top K Players of a Specific Team by Role\n");
    printf("5. Display All Players of Specific Role Across All Teams\n");
    printf("6. Exit\n");
    printf("Enter your choice: ");
}

int main() {
    init_teams_from_header();
    load_initial_players();

    while (1) {
        print_menu();
        int choice = read_int();
        if (choice == 1) interactive_add_player();
        else if (choice == 2) {
            printf("Enter Team ID: ");
            int tid = read_int();
            Team *t = get_team_by_id(tid);
            if (t) display_players_of_team(t);
        }
        else if (choice == 3) display_teams_by_avg_strike();
        else if (choice == 4) {
            printf("Enter Team ID: ");
            int tid = read_int();
            Team *t = get_team_by_id(tid);
            if (!t) continue;
            printf("Role (1-Batsman, 2-Bowler, 3-All-rounder): ");
            int r = read_int();
            printf("Enter K: ");
            int K = read_int();
            display_top_k_team_role(t, (Role)r, K);
        }
        else if (choice == 5) {
            printf("Enter Role (1-Batsman,2-Bowler,3-All-rounder): ");
            int r = read_int();
            display_all_players_role_across_teams((Role)r);
        }
        else if (choice == 6) {
            free_all_memory();
            return 0;
        }
    }
    return 0;
}
