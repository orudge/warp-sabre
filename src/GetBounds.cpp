
#include "ReadDelimitedFile.h"
#include <string.h>
#include <stdlib.h>

int GetBounds(class DelimitedFile &boundsFile, const char *filename, vector<string> &boundsOut, string &projType)
{
        boundsOut.clear();
        for (unsigned int i = 0; i < boundsFile.NumLines(); i++)
        {
                class DelimitedFileLine &line = boundsFile.GetLine(i);
                if (line.NumVals() < 4)
                        continue;
                if (strcmp(line[1].GetVals(), filename) != 0)
                        continue;

                // Read details from this line
                projType = line[0].GetVals();
                for (unsigned int j = 2; j < line.NumVals(); j++)
                {
                        // cout << strlen(line[j].GetVals()) << "," << line[j].GetVals() << endl;
                        boundsOut.push_back(line[j].GetVals());
                }
                return boundsOut.size();
        }
        return boundsOut.size();
}
