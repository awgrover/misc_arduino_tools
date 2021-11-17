/*
  Accumulate a histogram.
*/

template <typename T>
class Histogram {
  // T must be one of the numerics int, float, byte, etc
  public:
    T *buckets; // 0..n buckets, use count_at(T value) or bucket_value(int i)
    const int bucket_ct;
    const T low_value;
    const T high_value;
    const T bucket_width;

    Histogram(int bucket_ct, T low_value, T high_value) :
      bucket_ct(bucket_ct),
      low_value(low_value), 
      high_value(high_value),
      bucket_width( (high_value - low_value) / bucket_ct )
    {
      buckets = new T[bucket_ct];
      reset();
    }

    void reset() {
      memset ( buckets, 0, bucket_ct * sizeof(T) );
    }

    void value(T value) {
      int i = map( value, low_value, high_value, 0, bucket_ct);
      buckets[i] += 1;
    }

    int count_at(T value) { // the count of value's bucket
      int i = map( value, low_value, high_value, 0, bucket_ct);
      return buckets[i];
    }

    T bucket_value(int i) { // the smallest value that would be in bucket[i]
      return map( i, 0, bucket_ct, low_value, high_value);
    }
    T bucket_value_mid(int i) { // the mid value that would be in bucket[i]
      return map( i, 0, bucket_ct, low_value, high_value) + bucket_width/2;
    }
    // bucket_value_next(i) would be bucket_value(i+1). i.e. can't get max!
};
