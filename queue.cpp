#include<iostream>
#include<bits/stdc++.h>
using namespace std;


class Node{
    public:
    int value;
    Node* next;
    
    Node(int value){
        this->value = value;
        this->next = NULL;
    }

};


bool cas(atomic<Node*> &mem, Node* cmp, Node* with)
{
    //return __sync_val_compare_and_swap(mem, cmp, with);
    return mem.compare_exchange_strong(cmp, with,  memory_order_release, memory_order_relaxed);
    
    //non atomic cass
    // if(mem==cmp){
    //     mem = with;
    //     // cout<<mem->value;
    //     return true;
    // }
    
    // return false;

}


class Lock_Free_Queue{

    
    
    public:
    atomic<Node*> head;
    atomic<Node*> tail;
    // Lock_Free_Queue(Node* sentinel){
        
    //      head = sentinel;
    //      tail = sentinel;
    //      //cas(head, NULL, sentinel);
    // }
    //~Lock_Free_Queue();
    Lock_Free_Queue();
    Lock_Free_Queue(Node* sentinel) :head(sentinel), tail(sentinel)
    
    {}
    
    void enqueue(int value){
        cout<<"enque"<<endl;
        Node* t = new Node(value);
        
        while(true){
        
            Node* last = tail;
            Node* next = last->next;
            
            if(last == tail){
                if(next == NULL){
                  
                   //if(cas(last->next,NULL,t))
                  // {cas(tail, last, t);
                   //cout<<last->value<<endl;
                   //return;}
                    Node* curr_node = head.load(memory_order_relaxed);
                    Node* t_1 = curr_node->next;
                    //cout<<t_1->value;
                    while(t_1!=NULL){
                    t_1 = t_1->next;   
                    }                
                   cas(tail, last, t);
                   return;
                    
                }else{
            
                    cas(tail, last, next);
                }  
            }  
        
        }
        Node* curr_node = head.load(memory_order_relaxed);
        Node* t_1 = curr_node->next;
         cout<<t_1->value; 
         while(t_1!=NULL){
         cout<<"Loudu"<<endl;
         cout<<t_1->value<<" ";
         t_1 = t_1->next;
     }
        
    }   
    
    
    int deque(){
     
        while(true){
            cout<<"Dequeeee"<<endl;
            Node* first = head;
            Node* next = first->next;
            Node* last = tail;
            if(first == last){
                if(next == NULL)
                
                throw underflow_error("called deque on empty queue");
                //cout<<"called deque on empty queue";
                
                else{
                    cas(tail, last, next);
                }
            }else{
                int value = first->value;
                
                if(cas(head, first, next)){
                    return value;
                }        
            }
        }
        return 1;
        
    }

};
void printQueue(Lock_Free_Queue q){
    Node* curr_node = q.head.load(memory_order_relaxed);
    cout<<curr_node->value;
     Node* t = curr_node->next;
     //t = t->next;
    while(t!=NULL){

         cout<<t->value<<" ";
         t = t->next;
     }
    // if(t!=NULL){
     //cout<<t->value<<" "<<endl;
     //}
}

int main(){
    
    Node* sentinel = new Node(-1);
    Lock_Free_Queue q(sentinel);
    
q.enqueue(10);
printQueue(q);
q.enqueue(20);
printQueue(q);
q.enqueue(30);
printQueue(q);
q.deque();
printQueue(q);
q.enqueue(30);
printQueue(q);
q.enqueue(30);

q.deque();



    
}
