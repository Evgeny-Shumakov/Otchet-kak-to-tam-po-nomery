#include "rccregisters.hpp" //for rcc
#include "gpioaregisters.hpp" //for gpioa
#include "gpiocregisters.hpp" //for gpioc
#include "tim2registers.hpp" //for tim2

extern "C"

{
  int __low_level_init(void)
  {
    RCC::CR::HSION::Enable::Set ();
    while ( ! RCC::CR::HSIRDY::Enable::IsSet () )
    {
    };
    
      RCC::CFGR::SW::Hsi::Set();
      while ( ! RCC::CFGR::SWS::Hsi::IsSet () )
    {
    };
    RCC::AHB1ENR::GPIOAEN::Enable::Set(); // Podkluchaem GPIOA
    GPIOA::MODER::MODER5::Output::Set(); // Nastraivaem kak vihod (diod)
    
    RCC::AHB1ENR::GPIOCEN::Enable::Set(); // Podkluchaem GPIOC
    GPIOC::MODER::MODER5::Output::Set(); // Nastraivaem kak vihod (diod)
    GPIOC::MODER::MODER8::Output::Set(); // Nastraivaem kak vihod (diod)
    GPIOC::MODER::MODER9::Output::Set(); // Nastraivaem kak vihod (diod)
    GPIOC::MODER::MODER13::Input::Set(); // Nastraivaem kak vhod (knopka)
    
    RCC::APB1ENR::TIM2EN::Enable::Set(); //Podkluchaem taimer
    TIM2::CR1::URS::Overflow::Set(); //Taimer stavim na perepolnenie
    return 0;
  }
}

void delay(std::uint32_t delay) //Realizacia zaderzhki
{
  TIM2::ARR::Write(16'000 *delay);
  TIM2::SR::UIF::NoUpdate::Set();
  TIM2::CNT::Write(0);
  TIM2::CR1::CEN::Enable::Set();
  while(TIM2::SR::UIF::NoUpdate::IsSet())
    {
    }
};

 struct Button  //Realizuem knopku (chtobi nuzhno bilo tol'ko nazhimat')
 {
   static bool IsPressed() //Zadaem nazhatie
   {
     bool result = false;
     if (GPIOC::IDR::IDR13::Low::IsSet() )
     {
       while (GPIOC::IDR::IDR13::Low::IsSet() )
       {
         result = true;
       };
     }
     return result;
   }
 };

template <typename Port, std::uint32_t pinNum> 
struct Led //Realizacia perecluchenia cherez Toggle
{
  static void Toggle()
  {
    Port::ODR::Toggle(1<<pinNum);
  }
};

using Led1 = Led<GPIOC,5>;
using Led2 = Led<GPIOC,8>;
using Led3 = Led<GPIOC,9>;
using Led4 = Led<GPIOA,5>;

int main()
{
  for (; ;)
  {
      Led1::Toggle();
      delay(500); //500 ms
      Led2::Toggle(); 
      delay(1000); //1000 ms
      Led3::Toggle();
      delay(1500); //1500 ms
      Led4::Toggle();
      delay(500); //500 ms
  } 
  return 0;
}
