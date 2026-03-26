#include <stdio.h>
#include <stdlib.h>

struct person {
  int age;
  char name[20];
  float heigth;
};

void print_person(struct person *p) {
  printf("Stampo la persona...\n");
  printf("Età: %d, Nome: %s, Altezza: %f\n", p->age, p->name, p->heigth);
}

void print_people(struct person *p, int count) {
  for (int i = 0; i < count; i++) {
    print_person(&p[i]);
  }
}

void get_person_info_from_user(struct person *p) {
  int age;
  printf("Insert age: \n");
  scanf("%d", &age);
  p->age = age;

  printf("Insert heigth: \n");
  float height;
  p->heigth = height;
  scanf("%f", &height);

  printf("Insert name: \n");
  scanf("%19s", p->name);
}

int main() {
  int number_of_people;
  printf("Insert number of people\n");
  scanf("%d", &number_of_people);

  struct person *p = malloc(number_of_people * sizeof(struct person));
  for (int i = 0; i < number_of_people; i++) {
    printf("PERSON %d\n", i + 1);
    get_person_info_from_user(&p[i]);
  }

  print_people(p, number_of_people);
  free(p);

  return 0;
}
