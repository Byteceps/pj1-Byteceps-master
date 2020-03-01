//
// freq.cc
//
// This implements the operations for the unordered dictionary data
// structure with the type `freq::dict*` for Project 1 of the Spring
// 2020 offering of CSCI 221.  Specifically, we define the functions
// for a bucket hash table that stores a collection of words and their
// integer counts.
//
// It is described more within the README for this project.
//
// The functions it defines include
//    * `int hashValue(std::string,int)`: give the hash location for a key
//    * `freq::dict* freq::build(int,int)`: build a word count dictionary 
//    * `int freq::totalCount(freq::dict*)`: get the total word count
//    * `int freq::numKeys(freq::dict*)`: get number of words
//    * `void freq::increment(freq::dict*,std::string)`: bump a word's count 
//    * `int freq::getCount(freq::dict*,std::string)`: get the count for a word
//    * `freq::entry* freq::dumpAndDestroy(freq::dict*)`: get the word counts, sorted by frequency
//    * `void freq::rehash(freq::dict*)`: expand the hash table
//
// The top four are implemented already, the other four need to be written.
//

#include <string>
#include <iostream>
#include "freq.hh"


// * * * * * * * * * * * * * * * * * * * * * * *
//
// HELPER FUNCTIONS FOR CHOOSING HASH TABLE SIZE
//

// isPrime(n)
//
// Return whether or not the given integer `n` is prime.
//
bool isPrime(int n) {
  // Handle the obvious cases, including even ones.
  if ((n <= 2) || (n % 2 == 0)) {
    return (n == 2);
  }
  // Try several odd divisors.
  int d = 3;
  while (d*d <= n) {
    if (n % d == 0) {
      // It has a divisor. It's not prime.
      return false;
    }
    d += 2;
  }
  // No divisors. It's prime.
  return true;
}

// primeAtLeast(n)
//
// Return the smallest prime number no smaller
// than `n`.
//
int primeAtLeast(int n) {
  if (n <= 2) {
    return 2;
  }
  int p = 3;
  while (p < n || !isPrime(p)) {
    p += 2;
  }
  return p;
}

// * * * * * * * * * * * * * * * * * * * * * * *
//
// HELPER FUNCTIONS FOR COMPUTING THE HASH VALUE
//

// charToInt(c):
//
// Returns an integer between 0 and 31 for the given character. Pays
// attention only to letters, the contraction quote, "stopper" marks,
// and space.
//
//
int charToInt(char c) {
  if (c >= 'a' && c <= 'z') {
    return c - 'a' + 1;
  } else if (c == '.') {
    return 27;
  } else if (c == '!') {
    return 28;
  } else if (c == '?') {
    return 29;
  } else if (c == '\'') {
    return 30;
  } else if (c == ' ') {
    return 31;
  } else {
    return 0;
  }
}

// hashValue(key,tableWidth):
//
// Returns an integer from 0 to tableWidth-1 for the given string
// `key`. This serves as a hash function for a table of size
// `tableWidth`.  
//
// This method treats the string as a base-32 encoding of the integer
// it computes, modulo `tableWidth`. It relies on `charToInt` defined
// just above.
//
int hashValue(std::string key, int modulus) {
  int hashValue = 0;
  for (char c: key) {
    // Horner's method for computing the value.
    hashValue = (32*hashValue + charToInt(c)) % modulus;
  }
  return hashValue;
}


// * * * * * * * * * * * * * * * * * * * * * * *
//
// Operations on freq::dict, and other support functions.
//
namespace freq {

//    * `int hashValue(std::string,int)`: give the hash location for a key
//    * `freq::dict* freq::build(int,int)`: build a word count dictionary 
//    * `int freq::totalCount(freq::dict*)`: get the total word count
//    * `int freq::numKeys(freq::dict*)`: get number of words
//    * `void freq::increment(freq::dict*,std::string)`: bump a word's count 
//    * `int freq::getCount(freq::dict*,std::string)`: get the count for a word
//    * `freq::entry* freq::dumpAndDestroy(freq::dict*)`: get the word counts, sorted by frequency
//    * `void freq::rehash(freq::dict*)`: expand the hash table
  // buildBuckets(howMany):
  //
  // Return an array of buckets of length `howMany`.
  //
  bucket* buildBuckets(int howMany) {
    bucket* bs = new bucket[howMany];
    for (int i=0; i<howMany; i++) {
      bs[i].first = nullptr;
    }
    return bs;
  }

  // build(initialSize,loadFactor):
  //
  // Build a word count dictionary that is roughly the given size, and
  // maintains the given load factor in its hash table.
  //
  dict* build(int initialSize, int loadFactor) {
    dict* newD = new dict;
    newD->numIncrements = 0;
    newD->numEntries    = 0;
    newD->loadFactor    = loadFactor; 
    newD->numBuckets    = primeAtLeast(initialSize);
    newD->buckets       = buildBuckets(newD->numBuckets);
    return newD;
  }

  // numKeys(D):
  //
  // Gives back the number of entries stored in the dictionary `D`.
  //
  int numKeys(dict* D) {
    return D->numEntries;
  }


  // totalCount(D):
  //
  // Gives back the total of the counts of all the entries in `D`.
  //
  int totalCount(dict* D) {
    return D->numIncrements;
  }



//Returns length of a bucket entry linked list 
int length (bucket* b){
  if (b->first == nullptr){
    return 0;
  }
  int count = 0;
  entry* current = b->first;
  while (current != nullptr){
    count++;
    current = current->next;
  }
  return count;
}

  // increment(D,w):
  //
  // Adds one to the count associated with string `k` in `D`, possibly
  // creating a new entry.
  //

  void increment(dict* D, std::string k) {
    int hash = hashValue(k, D->numBuckets); //Correct index for k's bucket
  
    bucket* theBucket = &D->buckets[hash]; //Pointer to correct bucket

    //If the bucket is not empty
    if (theBucket->first !=nullptr){

      entry* current = theBucket->first;
      //Transverse through entry list to see if key has an entry
      //Increment that entry's count if found
      while (current != nullptr){
        if(current->word == k){
          current->count+= 1;
          return;
        }
        else{
          current = current->next;
        }
      }
    }

    //If the bucket is empty, create new entry for k
    else {
      entry* newEntry = theBucket->first;
      newEntry->word = k;
      newEntry->count = 1;
      newEntry->next = nullptr;
      theBucket->first = newEntry;
    }
  }
  

  // getCount(D,w):
  //
  // Gets the count associated with the word `k` in `D`.
  //
  int getCount(dict* D, std::string k) {
    int hash = hashValue(k, D->numBuckets); //Correct index for k's bucket
    bucket* theBucket = &D->buckets[hash]; //Pointer to correct bucket


    //If bucket is not empty
    if (theBucket->first != nullptr){

      //Transverse through entry list to see if key has an entry and return its count
      entry* current = theBucket->first;
      while (current != nullptr){
        if (current->word == k){
          return current->count;
        }
        else{
          current = current->next;
        }
        return 0;
      }
    }
    //Return 0 if bucket is empty  
    else {
      return 0;
    }
  }






















  // rehash(D):
  //
  // Roughly doubles the hash table of `D` and places its entries into
  // that new structure.
  //
  void rehash(dict* D) {
    // UNIMPLEMENTED
    return;
  }


  // dumpAndDestroy(D):
  //
  // Return an array of all the entries stored in `D`, sorted from
  // most to least frequent.
  //
  // Deletes all the heap-allocated components of `D`.
  //
  entry* dumpAndDestroy(dict* D) {
    // UNIMPLEMENTED
    entry* es = new entry[3];
    es[0].word = "hello";
    es[0].count = 0;
    es[0].next = nullptr;
    es[1].word = "world";
    es[1].count = 1;
    es[1].next = nullptr;
    es[2].word = "!";
    es[2].count = 2;
    es[2].next = nullptr;
    return es;
  }
} // end namespace freq

