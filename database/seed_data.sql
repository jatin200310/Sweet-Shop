USE sweet_shop;

-- Insert sample users
INSERT INTO users (username, password_hash, email, is_admin) VALUES
('admin', '$6$abcdefghij$ZWqO9z0QzX3xJ8K5L9M2N6P3Q7R4S1T8U2V5W9X3Y7Z0A4B8C1D5E9F2G6H0', 'admin@sweetshop.com', TRUE),
('customer1', '$6$abcdefghij$ZWqO9z0QzX3xJ8K5L9M2N6P3Q7R4S1T8U2V5W9X3Y7Z0A4B8C1D5E9F2G6H0', 'customer1@example.com', FALSE),
('customer2', '$6$abcdefghij$ZWqO9z0QzX3xJ8K5L9M2N6P3Q7R4S1T8U2V5W9X3Y7Z0A4B8C1D5E9F2G6H0', 'customer2@example.com', FALSE);

-- Insert sample sweets
INSERT INTO sweets (name, description, category, price, quantity) VALUES
('Dark Chocolate Bar', 'Premium dark chocolate with 70% cocoa', 'Chocolate', 2.99, 45),
('Milk Chocolate Truffle', 'Smooth milk chocolate center with cocoa coating', 'Chocolate', 1.49, 80),
('Gummy Bears Assorted', 'Colorful mix of fruit-flavored gummy bears', 'Gummies', 1.99, 120),
('Sour Gummy Worms', 'Sour-coated worm-shaped gummies', 'Gummies', 2.49, 60),
('Lollipop Cherry Burst', 'Large round lollipop with cherry flavor', 'Hard Candy', 0.99, 200),
('Lollipop Grape Swirl', 'Striped lollipop with grape and lime flavors', 'Hard Candy', 0.99, 180),
('Caramel Chew Delight', 'Soft caramel candy with smooth chew', 'Caramel', 1.29, 100),
('Salted Caramel Fudge', 'Rich fudge with sea salt and caramel swirl', 'Caramel', 2.79, 35),
('Mint Chocolate Chip', 'Creamy mint with dark chocolate chips', 'Chocolate', 3.49, 25),
('Strawberry Taffy', 'Chewy taffy with authentic strawberry taste', 'Taffy', 0.79, 250);

-- Insert sample purchases
INSERT INTO purchases (user_id, sweet_id, quantity, total_price) VALUES
(2, 1, 2, 5.98),
(2, 3, 1, 1.99),
(3, 5, 3, 2.97),
(3, 7, 2, 2.58),
(2, 9, 1, 3.49);

-- Insert sample audit log entries
INSERT INTO audit_log (user_id, action, target_type, target_id, details) VALUES
(1, 'CREATE', 'sweet', 1, 'Added new sweet: Dark Chocolate Bar'),
(1, 'CREATE', 'sweet', 2, 'Added new sweet: Milk Chocolate Truffle'),
(2, 'PURCHASE', 'sweet', 1, 'Purchased 2 units at $5.98 total'),
(3, 'PURCHASE', 'sweet', 5, 'Purchased 3 units at $2.97 total'),
(1, 'UPDATE', 'sweet', 7, 'Updated quantity to 100');