typedef struct _nameNode NameNode;

struct _nameNode{
	char* name;
	NameNode *next;
};

typedef struct _namelist{

	char* type;
	NameNode* name;
	unsigned int n;

}NameList;

enum {
	HUNTER = 0,
	GUARD,
	HANDMAIDEN,
	STABLEHAND,
	SPY,
	SCRIBER,
	WOODSMAN,
	BLACKSMITH
};

enum {
	CHARM = 0,
	CONCENTRATION,
	SCRIBE,
	ANIMALHANDLING,
	MEDICINE,
	COOKING,
	METALWORKING,
	WOODWORKING,
	WEAPONSKILL,
	THIEVERY,
	FARMING,
	SURVIVAL	
};

typedef struct Character{

	char type;

	char* name;
	char* religion;
	char* title;

	char age;
	char loyalty;
	char reputation;
	char fatigue;
	char wounds;
	char encumbrance;

	char corruption;
	// corruptionEffects;
	char mana;

	char iq;
	char st;
	char ch;
	char dx;

	//personalityTraits;
	//merits;
	//flaws;
	int skills[12];

	//equip;
	char protection;
	char magicprotection;
    
    struct Character *prev, *next;
} Character;

Character* generateCharacter(int type, char* nation);

int loadNames();
char* getRandomName(char* type);
