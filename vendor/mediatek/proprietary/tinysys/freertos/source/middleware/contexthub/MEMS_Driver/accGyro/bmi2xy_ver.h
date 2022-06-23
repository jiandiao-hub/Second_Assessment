/*!
 * @section LICENSE
 *
 * (C) Copyright 2011~2018 Bosch Sensortec GmbH All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *------------------------------------------------------------------------------
 *  Disclaimer
 *
 * Common: Bosch Sensortec products are developed for the consumer goods
 * industry. They may only be used within the parameters of the respective valid
 * product data sheet.  Bosch Sensortec products are provided with the express
 * understanding that there is no warranty of fitness for a particular purpose.
 * They are not fit for use in life-sustaining, safety or security sensitive
 * systems or any system or device that may lead to bodily harm or property
 * damage if the system or device malfunctions. In addition, Bosch Sensortec
 * products are not fit for use in products which interact with motor vehicle
 * systems.  The resale and/or use of products are at the purchaser's own risk
 * and his own responsibility. The examination of fitness for the intended use
 * is the sole responsibility of the Purchaser.
 *
 * The purchaser shall indemnify Bosch Sensortec from all third party claims,
 * including any claims for incidental, or consequential damages, arising from
 * any product use not covered by the parameters of the respective valid product
 * data sheet or not approved by Bosch Sensortec and reimburse Bosch Sensortec
 * for all costs in connection with such claims.
 *
 * The purchaser must monitor the market for the purchased products,
 * particularly with regard to product safety and inform Bosch Sensortec without
 * delay of all security relevant incidents.
 *
 * Engineering Samples are marked with an asterisk (*) or (e). Samples may vary
 * from the valid technical specifications of the product series. They are
 * therefore not intended or fit for resale to third parties or for use in end
 * products. Their sole purpose is internal client testing. The testing of an
 * engineering sample may in no way replace the testing of a product series.
 * Bosch Sensortec assumes no liability for the use of engineering samples. By
 * accepting the engineering samples, the Purchaser agrees to indemnify Bosch
 * Sensortec from all claims arising from the use of engineering samples.
 *
 * Special: This software module (hereinafter called "Software") and any
 * information on application-sheets (hereinafter called "Information") is
 * provided free of charge for the sole purpose to support your application
 * work. The Software and Information is subject to the following terms and
 * conditions:
 *
 * The Software is specifically designed for the exclusive use for Bosch
 * Sensortec products by personnel who have special experience and training. Do
 * not use this Software if you do not have the proper experience or training.
 *
 * This Software package is provided `` as is `` and without any expressed or
 * implied warranties, including without limitation, the implied warranties of
 * merchantability and fitness for a particular purpose.
 *
 * Bosch Sensortec and their representatives and agents deny any liability for
 * the functional impairment of this Software in terms of fitness, performance
 * and safety. Bosch Sensortec and their representatives and agents shall not be
 * liable for any direct or indirect damages or injury, except as otherwise
 * stipulated in mandatory applicable law.
 *
 * The Information provided is believed to be accurate and reliable. Bosch
 * Sensortec assumes no responsibility for the consequences of use of such
 * Information nor for any infringement of patents or other rights of third
 * parties which may result from its use.
 *

 */


/*==============================================================================
   DESCRIPTION

   Impelements the driver for the Bosch BMI2XY

   EDIT HISTORY FOR FILE


when        who      what
----------  ---      -----------------------------------------------------------
08/09/2018  M  v0.0.0.2  :support the step counter
08/16/2018  M  v0.0.1.0  :update the config file to 0.6.0(bmi261), 0.4.0(bmi260)
08/29/2018  M  v0.0.1.0  :add the debug trace
10/11/2018  M  v0.0.1.1  :fix the coding style
09/17/2019  M  v0.0.2.1  :support the bmi220
09/23/2019  M  v0.0.3.1  :1 update the latest config file BMI220_v2.38.0_FW_4.12

10/11/2019  M  v0.1.3.1  :1 support the bmi260
                          2 bmi260_q_v2.37.0_Fw4.8
10/21/2019  M  v0.1.3.2  :1 support the bmi220 step counter
11/26/2019  M  v0.1.3.3  :1 optimize the code size
02/13/2020  M  v0.1.3.5  :1 add the debug interface
02/14/2020  M  v0.1.3.6  :1 fix the bug of acc power off
02/17/2020  M  v0.1.3.7  :1 backup and restore the register val in selftest function
02/19/2020  M  v0.1.3.8  :1use the chip id support the bmi220 and bmi260
03/03/2020  M  v0.1.3.9  :1support the bmi260 ois function
                                               2 add the switch for MT6885 platfrom adn mt6771
03/04/2020  M  v0.1.3.a  :1 add the switch for both MT6885 MT6771 for bmi220
03/11/2020  M  v0.1.3.b  :1 clear the spare3 adn asda_pupsel bit in the register 0x68
03/16/2020  M  v0.1.3.c  :1 update the bmi260 mtk driver code
03/30/2020  M  v0.1.3.d  :1
                                                MTK do not support the burst read length more than 2000,
                                                bmi260 fifo is 2048 bytes
                                                once detect the fifo length more than 1024, this will clear the fifo data ,
                                                make sure the interrupt pin can become low
                                                also here set the read length 1024 bytes to avoid the clear the fifo not finished

03/30/2020  M  v0.1.3.e  :1 support bmi160 and bmi220 in one driver code
04/01/2020  M  v0.1.3.f  :1 check the watermark less than the 700bytes
04/09/2020  M  v0.1.4.0  :1 set the fifo lenth to 896
                          2 oprimize the power delay
05/25/2020  M  v0.1.4.1  :1 update the config file to bmi220_v2.47.0

============================================================================*/
#define BMI2XY_MTK_HUB_DRIVER_VER 0x0141
