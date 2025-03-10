export function throttled(mainFunction: Function, delay: number) {
  let timerFlag: number | null = null; // Variable to keep track of the timer

  // Returning a throttled version
  return (...args: unknown[]) => {
    if (timerFlag === null) {
      // If there is no timer currently running
      mainFunction(...args); // Execute the main function
      timerFlag = setTimeout(() => {
        // Set a timer to clear the timerFlag after the specified delay
        timerFlag = null; // Clear the timerFlag to allow the main function to be executed again
      }, delay);
    }
  };
}
