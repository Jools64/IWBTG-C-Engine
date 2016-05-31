#define MAX_VALUE_LENGTH 32
#define MAX_VALUES_PER_SCRIPT 32

typedef enum ScriptValueType
{
    ScriptValueType_number,
    ScriptValueType_string
} ScriptValueType;

typedef struct ScriptValue ScriptValue;
typedef struct ScriptValue
{
    char key[MAX_VALUE_LENGTH];
    ScriptValueType type;
    union
    {
        char string[MAX_VALUE_LENGTH];
        double number;
    };
} ScriptValue;

typedef struct ScriptState
{
    char* source;
    int sourceLength,
        readPosition;
    ScriptValue values[MAX_VALUES_PER_SCRIPT];
    int valueCount;
} ScriptState;

typedef enum _TokenType // A file included in SDL_opengl.h also defines a TokenType enum. -_-
{
    TokenType_identifier,
    TokenType_number,
    TokenType_assignment,
    TokenType_endOfFile
} _TokenType;

#define MAX_TOKEN_LENGTH MAX_VALUE_LENGTH
typedef struct Token
{
    _TokenType type;
    char value[MAX_TOKEN_LENGTH];
} Token;

double scriptReadNumber(ScriptState* ss, char* name, double defaultValue)
{
    for(int i = 0; i < ss->valueCount; ++i)
        if(ss->values[i].type == ScriptValueType_number && strcmp(ss->values[i].key, name) == 0)
            return ss->values[i].number;
    return defaultValue;
}

bool charIsWhiteSpace(char c)
{
    return (c == ' ' || c == '\n' || c == '\r' || c == '\t');
}

bool charIsNumeric(char c)
{
    return (c >= '0' && c <= '9');
}

bool charIsAlphabetic(char c)
{
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

bool charIsIdentifierLegal(char c)
{
    return (charIsNumeric(c) || charIsAlphabetic(c) || c == '-' || c == '_');
}


bool getCurrentCharacter(ScriptState* s, char** c)
{
    if(s->readPosition == s->sourceLength-1)
        return 0;
    
    *c = &s->source[s->readPosition];
    return 1;
}

bool getNextCharacter(ScriptState* s, char** c)
{
    if(s->readPosition == s->sourceLength-1)
        return 0;
    
    *c = &s->source[++s->readPosition];
    return 1;
}

void tokenError(char c)
{
    printf("Error: Invalid token, got character: %c / %d\n", c, (int)c);
}

#define EOFC 3

bool getNextToken(ScriptState* s, Token* t)
{
    int readState = 0;
    char* charPointer, 
        * lastCharPointer;
    char c;
    
    char* valueStart = 0;
    int valueLength = 0;
    
    if(getCurrentCharacter(s, &charPointer))
        c = *charPointer;
    else 
        c = EOFC; // End of file
    
    
    while(true)
    {
        switch(readState)
        {
            case 0: // Start
                if(charIsWhiteSpace(c))
                {
                    // Nothing
                }
                else if(charIsAlphabetic(c) || c == '-' || c == '_') // Start of identifier
                {
                    valueStart = charPointer;
                    readState = 1;
                }
                else if(charIsNumeric(c))
                {
                    valueStart = charPointer;
                    readState = 2;
                }
                else if(c == '=')
                {
                    readState = 4;
                }
                else if(c == EOFC)
                {
                    t->type = TokenType_endOfFile;
                    return false;
                }
                else
                    tokenError(c);
                break;
                
            case 1: // Identifier
                if(charIsIdentifierLegal(c))
                {
                    valueLength++;
                }
                else
                {
                    t->type = TokenType_identifier;
                    readState = 100;
                    continue;
                }
                    
                break;
                
            case 2: // Integer
                if(charIsNumeric(c))
                {
                    valueLength++;
                }
                else if(c == '.')
                {
                    valueLength++;
                    readState = 3;
                }
                else if(charIsWhiteSpace(c) || c == EOFC)
                {
                    t->type = TokenType_number;
                    readState = 100;
                    continue;
                }
                else
                    tokenError(c);
                    
                break;
                
            case 3: // Real
                if(charIsNumeric(c))
                {
                    valueLength++;
                }
                else
                {
                    t->type = TokenType_number;
                    readState = 100;
                    continue;
                }

                break;
                
            case 4: // Assignment
                // Check here for ==, etc.
                t->type = TokenType_assignment;
                return true;
                break;
                
            case 100: // Copy value
                strncpy(t->value, valueStart, 
                            min(valueLength+1, MAX_TOKEN_LENGTH));
                t->value[min(valueLength+1, MAX_TOKEN_LENGTH)] = '\0';
                return true;
        }
        
        lastCharPointer = charPointer;
        
        if(getNextCharacter(s, &charPointer))
            c = *charPointer;
        else 
            c = EOFC; // End of file
    }
    
    return true;
}

ScriptState parseScript(char* s)
{
    ScriptState state;
    state.source = s;
    state.sourceLength = strlen(s);
    state.readPosition = 0;
    state.valueCount = 0;
    
    Token token;
    getNextToken(&state, &token);
    while(token.type != TokenType_endOfFile)
    {
        if(token.type == TokenType_identifier)
        {
            if(state.valueCount < MAX_VALUES_PER_SCRIPT)
            {
                ScriptValue* sv = &state.values[state.valueCount++];
                strcpy(sv->key, token.value);
                getNextToken(&state, &token);
                if(token.type == TokenType_assignment)
                {
                    getNextToken(&state, &token);
                    if(token.type == TokenType_number)
                    {
                        sv->type = ScriptValueType_number;
                        sv->number = atof(token.value);
                    }
                }
            }
            else
            {
                printf("Error: Ran out of room for values when parsing script.");
            }
        }
        
        getNextToken(&state, &token);
    }
    
    return state;
}