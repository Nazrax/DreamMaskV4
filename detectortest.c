#include <stdio.h>
#include <string.h>

#include "detector.h"

extern reading_data_t *leftData, *rightData;

int main(int argc, char** argv) {
  detector_init();
  
  FILE *fp = fopen (argv[1], "r");

  char line[70];

  /* threshold, movementSum, movementCount, ticksSinceMovement, cooldown, dreaming */

  float minutes;
  int left, right, count, buttons;
  bool_t leftDream, rightDream, inDream;
  float dreamStart;
  while (NULL != fgets(line, sizeof(line), fp)) {
    count = sscanf(line, "%g %*s %d %*s %d %d", &minutes, &buttons, &left, &right);
    detector_update(left, right);
    printf("%010.6f %f %3d %4d %4d %4d %4d %4d %4d %4d %4d\n", 
	   minutes, 
	   minutes * 60,
	   buttons,
	   left,
	   right,
	   leftData->lastMovement, 
	   rightData->lastMovement, 
	   leftData->threshold,
	   rightData->threshold,
	   leftData->movementCount,
	   rightData->movementCount
	   );
    if (inDream) {
      if (!leftData->dreaming && !rightData->dreaming) {
	fprintf(stderr, "%03.1f: Dream ended (%02.1f)\n\n", minutes, minutes-dreamStart);
      }
    } else {
      if (leftData->dreaming) {
	fprintf(stderr, "%03.1f: Dream started (left)\n", minutes);
	dreamStart = minutes;
      } else if (rightData->dreaming) {
	fprintf(stderr, "%03.1f: Dream started (right)\n", minutes);
	dreamStart = minutes;
      }
    }

    inDream = leftData->dreaming || rightData->dreaming;
  }

  fclose(fp);

  return 0;
}
