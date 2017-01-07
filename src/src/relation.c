#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "../include/relation.h"

relation_t* relation__create(relation_page_t* header,
                             const char* relation_name,
                             key_t key_type,
                             int rec_len)
{
  relation_t* new_relation = NULL;

  // Find an empty relation space
  for (int i=0; i<RELATION_PAGE_SIZE; i++){
    if(header->relations[i].root.leaf == NULL) {
      new_relation = &(header->relations[i]);
      break;
    }
  }

  if(new_relation == NULL){
    fprintf(stderr, "This page is full!");
    return NULL;
  }

  new_relation->ktype = key_type;
  new_relation->key_length = (key_type==TYPE_INT ? 4 : 10);
  new_relation->record_length = rec_len;

  new_relation->root.leaf = calloc(1, sizeof(leaf_page_t));
  new_relation->page_header = calloc(1, sizeof(directory_page_t));

  strncpy(new_relation->name, relation_name, RELATION_NAME_SIZE);

  return new_relation;
}


data_entry_t relation__insert(relation_t* relation,
                              index_t key,
                              const char* remained_record)
{
  // Insert the record to pages and get RID
  return dpage__insert_record(relation->page_header,
                       relation->record_length,
                       relation->ktype,
                       key,
                       remained_record);
  // Then insert the RID and key to B+Tree
}


void relation__find(relation_t* relation, uint16_t pid, uint16_t slot_number) {
  if(relation->page_header == NULL){
    fprintf(stderr, "This relation is empty!\n");
    return;
  }

  directory_page_t* cur_dirct = relation->page_header;
  int dirct_id = pid / DIRECTORY_ENTRY_NUM;
  int dirct_entry_id = pid % DIRECTORY_ENTRY_NUM;

  while(cur_dirct!=NULL && cur_dirct->pid_base != dirct_id*DIRECTORY_ENTRY_NUM)
    cur_dirct = cur_dirct->next;

  if(cur_dirct==NULL){
    fprintf(stderr, "Warning: Cannot find the record with <pid, slot#>: <%d, %d>\n", pid, slot_number);
    return;
  } else {
    printf("Pid: %d\n", pid);
    rpage__show_record(&cur_dirct->entry[dirct_entry_id], slot_number, relation->record_length, relation->ktype);
  }

}


void relation_display_info(relation_t* relation)
{

  if(relation->page_header == NULL){
    fprintf(stderr, "This relation is empty!\n");
    return;
  }

  printf("New relation name: %s\n", relation->name);
  printf("----> Key type: %s\n",
         relation->ktype == TYPE_INT ? "Integer" : "String");
  printf("----> Record length: %d\n", relation->record_length);
  printf("----> Root Address: %p, size: %lu\n",
         relation->root.leaf,
         sizeof(*(relation->root.leaf)));
  printf("----> Directory Address: %p, size: %lu\n\n",
         relation->page_header,
         sizeof(*(relation->page_header)));
}
