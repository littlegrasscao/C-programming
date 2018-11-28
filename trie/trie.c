#include "trie.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

// The structure representing the trie
struct trie_data_t{
      int number;
      trie_pos_t head;
};

// A structure representing a trie node
struct trie_node_t{
      void * value;     //store value
      char letter;      //store node letter
      trie_pos_t child;  //to the first child node
      trie_pos_t next;  //to the next sibling node
      trie_pos_t parent;  //to the parent node
      trie_pos_t previous;  //to the previous sibling node
};

/// Visit every key in the trie
bool trie_walk (trie_t trie, trie_walk_t walkfunc, void * priv){
      //check if there is key in trie
      if (trie == TRIE_INVALID || trie->head == TRIE_INVALID_POS)
            return true;

      int count = trie->number; //number of keys
      bool walk = true; //return of walkfunc
      char* key;        //key pass to walkfunc
      int level = 1;    //level of trie
      trie_pos_t pos;   //pos pass to walkfunc
      trie_pos_t tmp = trie->head;  //first node

      key = (char*)malloc(sizeof(char));  //malloc key
      //continue visit if walk is true
      while(walk && count>0){
            //get key
            //keep visiting to the leaf
            while (tmp->child != TRIE_INVALID_POS && walk && count>0){
                  key = (char*)realloc(key, sizeof(char)*(level+1));
                  key[level-1] = tmp->letter;
                  if (tmp->value != (void*)0){
                        key[level] = '\0';
                        pos = tmp;
                        //call walkfunc and decreament number of count
                        walk = walkfunc(trie, pos, key, priv);
                        count--;
                  }
                  tmp = tmp->child;
                  level++;
            }

            //break if violate
            if(!walk || count==0){
                  break;
            }

            //leaf node must be a valid key
            key = (char*)realloc(key, sizeof(char)*(level+1));
            key[level-1] = tmp->letter;
            key[level] = '\0';
            pos = tmp;
            //call walkfunc and decreament number of count
            walk = walkfunc(trie, pos, key, priv);
            count--;

            //break if violate
            if(!walk || count==0){
                  break;
            }

            //move to parent if no next
            while (tmp->next == TRIE_INVALID_POS){
                  tmp = tmp->parent;
                  level--;
            }
            
            tmp = tmp->next; //move to next
      }                   

      free(key);
      //check if traverse whole trie
      if(count == 0)
            return true;
      else
            return false;
}


/// Free trie
void trie_destroy (trie_t trie, trie_free_t freefunc){
      //if trie is invalid
      if (trie == TRIE_INVALID){
            return;
      }
      //if no head node
      if (!trie->head){
            free(trie);
            return;
      }

      trie_pos_t tmp = trie->head;
      trie_pos_t removekey;
      //if need to use freefunc
      bool fun;
      if (freefunc)
            fun = true;
      else
            fun = false;

      
      while (tmp){
            //get the leaf node
            while(tmp->child){
                  tmp = tmp->child;
            }
            //if node has previous and next, remove node only
            if(tmp->previous != TRIE_INVALID_POS  &&  tmp->next != TRIE_INVALID_POS){
                  removekey = tmp;
                  tmp->next->previous = tmp->previous;
                  tmp->previous->next = tmp->next;
                  tmp = tmp->next;
                  //call function
                  if (fun && removekey->value){
                        freefunc(removekey->value);
                  }
                  free(removekey);
            }
            //if node only have next
            else if(tmp->previous == TRIE_INVALID_POS  &&  tmp->next != TRIE_INVALID_POS)
            {
                  removekey = tmp;
                  //if still have parent
                  if(tmp->parent){
                        tmp->parent->child = tmp->next;
                  }
                  //if first node
                  else{
                        trie->head = tmp->next;
                  }
                  tmp->next->previous = TRIE_INVALID_POS;
                  tmp = tmp->next;
                  //call function
                  if (fun && removekey->value){
                        freefunc(removekey->value);
                  }
                  free(removekey);
            }   
            //if node only have previous
            else if(tmp->previous != TRIE_INVALID_POS  &&  tmp->next == TRIE_INVALID_POS)
            {
                  removekey = tmp;
                  tmp->previous->next = TRIE_INVALID_POS;
                  tmp = tmp->previous;
                  //call function
                  if (fun && removekey->value){
                        freefunc(removekey->value);
                  }
                  free(removekey);
            }    
            //if no previous and no next
            else{
                  removekey = tmp;
                  //if no parent, means it is head node
                  if(tmp->parent == TRIE_INVALID_POS)
                  {
                        if (fun && removekey->value){
                              freefunc(removekey->value);
                        }
                        free(tmp);
                        trie->head = TRIE_INVALID_POS;
                        free(trie);
                        return;
                  }
                  //else
                  tmp = tmp->parent;
                  tmp->child = TRIE_INVALID_POS;
                  if (fun && removekey->value){
                        freefunc(removekey->value);
                  }
                  free(removekey);                  
            }    
      }
}

/// Get value associated with a key
/// NOTE: the pos was obtained by a call to trie_insert or trie_find
void * trie_get_value (const trie_t trie, trie_pos_t pos){
      return pos->value;
}



/// Set value associated with a key
/// NOTE: the pos was obtained by a call to trie_insert or trie_find.
void trie_set_value (trie_t trie, trie_pos_t pos, void * value){
      pos->value = value;
}



/// Create a new empty trie
trie_t trie_new (){
      trie_pos_t createNode(char key, void* value);

      trie_t trie;
      trie = (trie_t)malloc(sizeof(struct trie_data_t));
      trie->number = 0;
      trie->head = TRIE_INVALID_POS;
      return trie;
}


/// Return the number of keys in the trie
unsigned int trie_size (const trie_t trie){
      return trie->number;
}

//create an empty trie node
trie_pos_t createNode(char key, void* value){
      trie_pos_t node;
      node = (trie_pos_t)malloc(sizeof(struct trie_node_t));
      node->parent = TRIE_INVALID_POS;
      node->child = TRIE_INVALID_POS;
      node->previous = TRIE_INVALID_POS;
      node->next = TRIE_INVALID_POS;
      node->letter = key;
      node->value = value; 
      return node;
}


/// Insert a key in the trie; 
bool trie_insert (trie_t trie, const char * str, void * newval, trie_pos_t * newpos){
      //check if str already in trie
      trie_pos_t find;
      find = trie_find(trie,str);

      //str length
      int len = strlen(str);
      if (find != TRIE_INVALID_POS || len==0){
            //check if newpos is null
            if(newpos){
                  *newpos = find;
            }
            return false;
      }
            
      trie_pos_t tmp = trie->head; //create a tmp pointer to head

      for(int i=0; i<len; i++)
      {
            //if tmp == null, the rest of str goes to its child
            if (tmp == TRIE_INVALID_POS){
                  tmp = createNode(str[i],(void*)0);
                  if(trie->head == TRIE_INVALID_POS){
                        trie->head = tmp;
                  }
                  i++;
                  //continue adding to its child
                  while (i<len){
                        tmp->child = createNode(str[i],(void*)0);
                        tmp->child->parent = tmp;
                        tmp = tmp->child;
                        i++;
                  }
                  break;
            }
            //check if str[i] is already stored
            while(tmp->letter != str[i]){
                  if(tmp->next == TRIE_INVALID_POS)
                        break;
                  tmp = tmp->next;
            }
            //if existed already, move to the children
            if (tmp->letter == str[i])
            {
                  if (tmp->child != TRIE_INVALID_POS){
                        tmp = tmp->child;
                        continue;
                  }   
            }
            else{
                  //malloc a next node
                  tmp->next = createNode(str[i],(void*)0);
                  tmp->next->previous = tmp; //move to next
                  tmp->next->parent = tmp->parent; //same parent
                  tmp = tmp->next;
            }
            i++;
            //continue build child
            while (i<len){
                  tmp->child = createNode(str[i],(void*)0);
                  tmp->child->parent = tmp;
                  tmp = tmp->child;
                  i++;
            }
            break;                  
            
      }

      find = trie_find(trie,str);      //find the key node
      
      find->value = newval; //set value to node

      trie->number++;         //increment number
      //check if newpos is null
      if(newpos){
            *newpos = find;
      }      
      return true;
}

/// Find a key in a trie
/// Returns the position or TRIE_INVALID_POS if the key could not be found.
trie_pos_t trie_find (const trie_t trie, const char * key){
      if (strlen(key) == 0)
            return TRIE_INVALID_POS;


      trie_pos_t tmp = trie->head; //create a tmp pointer to head
      //go through key
      for(int i=0; i<strlen(key); i++)
      {
            //if tmp == null
            if (tmp == TRIE_INVALID_POS){
                  return TRIE_INVALID_POS;
            }

            //check if str[i] is already stored
            while(tmp->letter != key[i]){
                  if(tmp->next == TRIE_INVALID_POS)
                        break;
                  tmp = tmp->next;
            }
            //if existed already, move to the children
            if (tmp->letter == key[i])
            {
                  //if it is the last letter in key
                  if(i == strlen(key)-1)
                        return tmp;
                  tmp = tmp->child;
            }
            //if not exist, return invalid
            else{
                  return TRIE_INVALID_POS;
            }
      }
      return tmp;
}


/// Remove a key from a trie
bool trie_remove (trie_t trie, const char * key, void ** data){
      trie_pos_t tmp = trie_find(trie,key);

      //if not find the key
      if (tmp == TRIE_INVALID_POS || !tmp->value){
            return false;
      }

      //if find the key, reaplace data first
      if(data)
            *data = tmp->value;
      trie->number--; //decrease number

      trie_pos_t removekey; 
      while(tmp){
            //if node still have child, remove value inside only
            if(tmp->child != TRIE_INVALID_POS){
                  tmp->value = (void*) 0;
                  break;
            }
            //if node has previous and next, remove node only
            else if(tmp->previous != TRIE_INVALID_POS  &&  tmp->next != TRIE_INVALID_POS)
            {
                  removekey = tmp;
                  tmp->previous->next = tmp->next;
                  tmp->next->previous = tmp->previous;
                  free(removekey);
                  break;
            }
            //if node only have next
            else if(tmp->previous == TRIE_INVALID_POS  &&  tmp->next != TRIE_INVALID_POS)
            {
                  removekey = tmp;
                  //if still have parent
                  if(tmp->parent){
                        tmp->parent->child = tmp->next;
                  }
                  //if first node
                  else{
                        trie->head = tmp->next;
                  }
                  tmp->next->previous = TRIE_INVALID_POS;
                  free(removekey);
                  break;
            }   
            //if node only have previous
            else if(tmp->previous != TRIE_INVALID_POS  &&  tmp->next == TRIE_INVALID_POS)
            {
                  removekey = tmp;
                  tmp->previous->next = TRIE_INVALID_POS;
                  free(removekey);
                  break;
            }    
            //if no previous and no next
            else{
                  removekey = tmp;
                  //if not the head node
                  if(tmp->parent){
                        tmp = tmp->parent;
                        tmp->child = TRIE_INVALID_POS;
                        free(removekey); 
                        if(tmp->value)     
                              break;
                  }
                  //if is the head node
                  else{
                        free(removekey);
                        trie->head = TRIE_INVALID_POS;
                        break;
                  }
            }       
      }
      return true;
}

