#include <stdio.h>
#include <string.h>

#include "detector.h"

extern reading_data_t *leftData, *rightData;

int main(int argc, char** argv) {
  detector_init();
  
  FILE *fp = fopen (argv[1], "r");

  char line[100];

  /* threshold, movementSum, movementCount, ticksSinceMovement, cooldown, dreaming */

  float minutes;
  int left, right, count, buttons;
  bool_t leftDream, rightDream, inDream;
  float dreamStart;
  char time[15];
  while (NULL != fgets(line, sizeof(line), fp)) {
    count = sscanf(line, "%g %*s %d %s %d %d", &minutes, &buttons, time, &left, &right);
    detector_update(left, right);
    printf("%010.5f %010.3f %3d %4d %4d %4d %4d %4d %4d %4d %4d %4d %4d %2d %2d %1d %1d\n", 
	   minutes, 
           minutes * 60,
	   buttons,
	   left,
	   right,
	   leftData->lastMovement, // 6
	   rightData->lastMovement,  // 7
	   leftData->threshold, // 8
	   rightData->threshold, // 9
	   leftData->movementCount, // 10
	   rightData->movementCount, // 11
           leftData->ticksSinceMovement, // 12
           rightData->ticksSinceMovement, // 13
           leftData->cooldown, // 14
           rightData->cooldown, // 15
           leftData->dreaming, // 16
           rightData->dreaming // 17
	   );
    if (inDream) {
      if (!leftData->dreaming && !rightData->dreaming) {
	//fprintf(stderr, "%03.1f: Dream ended (%02.1f)\n\n", minutes, minutes-dreamStart);
	fprintf(stderr, "%s: Dream ended (%02.1f)\n\n", time, minutes-dreamStart);
      }
    } else {
      if (leftData->dreaming) {
	fprintf(stderr, "%s: Dream started (left)\n", time);
	//fprintf(stderr, "%03.1f: Dream started (left)\n", minutes);
	dreamStart = minutes;
      } else if (rightData->dreaming) {
	fprintf(stderr, "%s: Dream started (right)\n", time);
	//fprintf(stderr, "%03.1f: Dream started (right)\n", minutes);
	dreamStart = minutes;
      }
    }

    inDream = leftData->dreaming || rightData->dreaming;
  }

  fclose(fp);

  return 0;
}
