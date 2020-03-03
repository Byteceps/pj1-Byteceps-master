 if (firstEntry->next == nullptr){
            //Case when bucket has one entry
            newArr[i].word = firstEntry->word;
            newArr[i].count = firstEntry->count;
            newArr[i].next = nullptr;
            //Add heap delete
            
          }

          //Transverse entry list with two consecutive pointers
          entry* follower = firstEntry;
          entry* leader = firstEntry->next;
          if (leader->next == nullptr) {
            //If entry list has two entries

            //Transfer first entry
            newArr[i].word = follower->word;
            newArr[i].count = follower->count;
            newArr[i].next = follower->next;
            //Transfer second entry
            newArr[i].next->word = leader->word;
            newArr[i].next->count = leader->count;
            newArr[i].next->next = nullptr;
          }

          while (leader->next != nullptr) {
            newArr[i] = follower


            }