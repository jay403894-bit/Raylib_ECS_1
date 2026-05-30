import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.concurrent.ForkJoinPool;
import java.util.concurrent.RecursiveTask;

public class ParallelMergeSort {

    public static void main(String[] args) {
        List<Integer> unsortedList = new ArrayList<>(Arrays.asList(38, 27, 43, 3, 9, 82, 10));
        System.out.println("Original List: " + unsortedList);

        ForkJoinPool forkJoinPool = new ForkJoinPool();
        List<Integer> sortedList = forkJoinPool.invoke(new MergeSortTask(unsortedList));
        System.out.println("Sorted List: " + sortedList);
    }

    static class MergeSortTask extends RecursiveTask<List<Integer>> {
        private final List<Integer> elements;

        public MergeSortTask(List<Integer> elements) {
            this.elements = new ArrayList<>(elements); // Create a copy to avoid modifying original list
        }

        @Override
        protected List<Integer> compute() {
            if (elements.size() <= 1) {
                return elements; // Base case: already sorted
            } else {
                int mid = elements.size() / 2;
                List<Integer> leftHalf = elements.subList(0, mid);
                List<Integer> rightHalf = elements.subList(mid, elements.size());

                // Fork subtasks for parallel execution
                MergeSortTask leftTask = new MergeSortTask(leftHalf);
                MergeSortTask rightTask = new MergeSortTask(rightHalf);

                leftTask.fork(); // Start the left task asynchronously
                // The current thread can continue with the right task or other work
                // For simplicity, we directly compute the right task here
                List<Integer> rightResult = rightTask.compute(); 

                List<Integer> leftResult = leftTask.join(); // Wait for the left task to complete and get its result

                return merge(leftResult, rightResult); // Merge the sorted halves
            }
        }

        private List<Integer> merge(List<Integer> left, List<Integer> right) {
            List<Integer> mergedList = new ArrayList<>();
            int i = 0, j = 0;

            while (i < left.size() && j < right.size()) {
                if (left.get(i) < right.get(j)) {
                    mergedList.add(left.get(i++));
                } else {
                    mergedList.add(right.get(j++));
                }
            }

            while (i < left.size()) {
                mergedList.add(left.get(i++));
            }

            while (j < right.size()) {
                mergedList.add(right.get(j++));
            }
            return mergedList;
        }
    }
}