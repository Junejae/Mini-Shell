// Junejae Kim
// 9/20/2020
// COP4600-20Fall 0001
// Assignment 3

#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <algorithm>
#include <vector>
#include <sstream>
#include <unistd.h>
#include<sys/wait.h>
#include<sys/types.h>
using namespace std;

// Global variables
bool done = false;
vector<string> tokens;
vector<string> hist;
vector<pid_t> process;

// Function list
void movetodir();
void whereami();
void history ();
void byebye();
void startProgram();
void backgroundProgram();
void exterminate();
void repeat();
void exterminateall();
void commandReader();

// Main function
int main()
{
  string rawStr;
  string token;

  cout << "Welcome to MYSH! Type valid commands.\n";

  while (!done)
  {
    cout << "\n# "; // prompt
    getline(cin, rawStr);
    
    // cout << rawStr << endl; // debug

    // Put command line in history
    hist.push_back(rawStr);

    stringstream copyStr(rawStr);
    
    while (getline(copyStr, token, ' '))
    {
      tokens.push_back(token);
    }

    /* 
    for (int i = 0; i < tokens.size(); i++)
      cout << tokens[i] << endl; // debug 
    */

    // read command
    // execute command
    commandReader();

    tokens.clear();
  }

  return 0;
}


//              //
//  Functions   //
//              //

// # movetodir directory
void movetodir()
{
  if (tokens.size() > 2)
  {
    cout << "Error: movetodir accepts only one directory as a parameter\n";
    return;
  }

  const char* buffer = tokens[1].c_str();
  int result = chdir(buffer);

  if (result == 0)
    cout << "The working directory is moved to: " << buffer << endl;
  else
    cout << "Error: invalid directory.\n";
  
  return;
}

// # whereami
void whereami()
{
  if (tokens.size() > 1)
  {
    cout << "Error: whereami has no additional parameters.\n";
    return;
  }

  char* buffer;
  buffer = getcwd(NULL, 0);

  if (buffer == NULL)
    cout << "Error: directory cannot declared.\n";
  else
    cout << "Current path: " << buffer << endl;
  
  free(buffer);

  return;
}

// # history [-c]
void history ()
{
  if (tokens.size() == 2)
  {
    if (tokens[1] == "-c")
    {
      hist.clear();
      cout << "History is cleared." << endl;
    }
    else cout << "---Unknown Parameter, use -c only---\n";
  }
  else if (tokens.size() == 1)
  {

    for (int i = 0; i < hist.size(); i++)
      cout << hist[i] << endl;
  }
  else
  {
    cout << "---Error: history accepts only one parameter---\n";
  }
  
  return;
}

// # byebye
void byebye()
{
    cout << "Ending MYSH...\n";
    done = true;
    return;
}

// # start program [parameters]
void startProgram()
{
  if (tokens.size() < 2)
  {
    cout << "Error: start needs at least one parameter to process.\n";
    return;
  }
  
  pid_t pid;
  int status;

  const char* path = tokens[1].c_str();
  vector<const char*> args;

  for (int i = 1; i < tokens.size(); i++)
  {
    args.push_back(tokens[i].c_str());
  }

  const char* const* cLikeArgs = args.data();

  pid = fork();

  if (pid == -1) // Error
  {
    cout << "Creating a child processed is failed.\n";
  }
  else if (pid == 0) // Child
  {
    int stat;

    if (tokens[1][0] == '/') // contains directory
    {
      stat = execv(path, const_cast<char* const *>(cLikeArgs));
    }
    else // program in working directory
    {
      stat = execvp(path, const_cast<char* const *>(cLikeArgs));
    }

    if (stat < 0)
    {
      cout << "\nCannot execute the program (" << stat << ")\n";
      exit(0);
    }
  }
  else // Parent
  {
    waitpid(pid, &status, 0);
  }

  return;
}

// # background program [parameters]
void backgroundProgram()
{
  if (tokens.size() < 2)
  {
    cout << "Error: background needs at least one parameter to process.\n";
    return;
  }
  
  pid_t pid;
  int status;

  const char* path = tokens[1].c_str();
  vector<const char*> args;

  for (int i = 1; i < tokens.size(); i++)
  {
    args.push_back(tokens[i].c_str());
  }

  const char* const* cLikeArgs = args.data();

  pid = fork();

  if (pid == -1) // Error
  {
    cout << "Creating a child processed is failed.\n";
  }
  else if (pid == 0) // Child
  {
    int stat;

    if (tokens[1][0] == '/') // contains directory
    {
      stat = execv(path, const_cast<char* const *>(cLikeArgs));
    }
    else // program in working directory
    {
      stat = execvp(path, const_cast<char* const *>(cLikeArgs));
    }

    if (stat < 0)
    {
      cout << "\nCannot execute the program (" << stat << ")\n";
      exit(0);
    }
  }
  else // Parent
  {
    waitpid(pid, &status, WNOHANG);

    cout << "Running [" << pid << "] in the backgroud.\n";

    process.push_back(pid);
  }

  return;
}

// # exterminate PID
void exterminate()
{
  if (tokens.size() < 2)
  {
    cout << "There should be a pid as a parameter.\n";
    return;
  }

  pid_t pid = atoi(tokens[1].c_str());

  int stat = kill(pid, SIGKILL);

  if (stat == 0)
  {
    cout << "The process [" << pid << "] is killed.\n";

    vector<pid_t>::iterator index = find(process.begin(), process.end(), pid);

    process.erase(index);
  }
  else
  {
    cout << "Error: cannot terminate a process.";
  }
  
  return;
}

// # repeat n command
void repeat()
{
  int n = atoi(tokens[1].c_str());

  if (n < 1)
  {
    cout << "Error: you can't repeat it less than 1 times.\n";
    return;
  }

  tokens.erase(tokens.begin());
  
  for (int i = 0; i < n; i++)
    backgroundProgram();
  
  return;
}

// # exterminateall
void exterminateall()
{
  if (tokens.size() > 1)
  {
    cout << "Error: exterminateall does not accept parameters.\n";
    return;
  }

  int size = process.size();

  if (size < 1)
  {
    cout << "Error: No processes is running.\n";
    return;
  }

  cout << "Murdering " << size << " processes: ";

  int stat;

  for (int i = 0; i < size; i++)
  {
    stat = kill(process[i], SIGKILL);

    if (stat == 0)
    {
      cout << process[i] << (((i + 1) < size) ? ", ": ".\n");
    }
    else
    {
      cout << "\nError: cannot terminate a process: " << process[i] << endl;
    }
  }
}

// Read command and execute
void commandReader()
{
  if (tokens[0] == "history") history();
  else if (tokens[0] == "byebye") byebye();
  else if (tokens[0] == "movetodir") movetodir();
  else if (tokens[0] == "whereami") whereami();
  else if (tokens[0] == "start") startProgram();
  else if (tokens[0] == "background") backgroundProgram();
  else if (tokens[0] == "exterminate") exterminate();
  else if (tokens[0] == "repeat") repeat();
  else if (tokens[0] == "exterminateall") exterminateall();
  else cout << "---Unknown Command---\n";

  return;
}
