#include<iostream>
#include<bits/stdc++.h>
using namespace std;


class Node{
    public:
    int value;
    Node* next;
    
    Node(int value){
        value = value;
        next = NULL;
    }

};

bool cas(Node *mem, Node* with, Node* cmp)
{
    return __sync_val_compare_and_swap(mem, cmp, with);
}


class Lock_Free_Queue{

    
    
    public:
    Node* head;
    Node* tail;
    Lock_Free_Queue(){
        Node* sentinel = new Node(-1);
        Node* head = sentinel;
        Node* tail = sentinel;
    }
    
    void enqueue(int value){
        
        Node* t = new Node(value);
        
        while(true){
        
            Node* last = tail;
            Node* next = last->next;
            
            if(last == tail){
                if(next == NULL){
                   
                   if(cas(last->next,NULL,t))
                   {cas(tail, last, t);
                   return;}
                    
                }else{
            
                    cas(tail, last, next);
                }  
            }  
        
        }
        
    }   
    
    
    int deque(){
     
        while(true){
            Node* first = head;
            Node* next = first->next;
            Node* last = tail;
            
            if(first == last){
                if(next == NULL)
                
                //throw std::underflow_error();
                throw "called deque on empty queue";
                
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
    Node* t = q.head;
    while(t != q.tail){
        cout<<t->value<<" ";
        t = t->next;
    }
    cout<<t->value<<" "<<endl;
}

int main(){
    
    Lock_Free_Queue q;
    
    q.enqueue(10);
    printQueue(q);
    q.enqueue(20);
    printQueue(q);
    q.enqueue(30);
    printQueue(q);
    q.deque();
    printQueue(q);
    
}





